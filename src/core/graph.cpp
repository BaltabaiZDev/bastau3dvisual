#include "graph.h"
#include <random>
#include <cmath>
#include <algorithm>

static std::mt19937& rng() {
    static thread_local std::mt19937 gen{ std::random_device{}() };
    return gen;
}

// Сфера ішіндегі нүктені генерациялау (біркелкі)
static glm::vec3 randomInSphere(float R) {
    std::uniform_real_distribution<float> U(0.0f, 1.0f);
    std::uniform_real_distribution<float> Uang(0.0f, 1.0f);

    // Конус/сфера параметризациясы
    float u = U(rng());
    float v = Uang(rng());
    float w = Uang(rng());

    float theta = 2.0f * 3.14159265358979323846f * v; // [0, 2pi]
    float z = 2.0f * w - 1.0f;                         // [-1, 1]
    float r = std::sqrt(std::max(0.0f, 1.0f - z*z));   // circle radius at z

    // Көлем бойынша біркелкі таралу үшін радиус^3 ~ U
    float rad = R * std::cbrt(u);

    return rad * glm::vec3(r*std::cos(theta), z, r*std::sin(theta));
}

Node Graph::makeRandomNode() {
    Node nd{};
    nd.id  = nextId++;
    nd.pos = glm::vec3(0.0f);
    nd.vel = glm::vec3(0.0f);
    nd.roamRadius = 0.12f;
    nd.state = NodeState::Pending; // жаңа тапсырма – Pending

    // N-ге тәуелді кеңістік радиусы (тығыздық тұрақты болсын)
    int n = std::max(1, (int)nodes.size());
    float worldR = std::max(1.2f, 0.28f * std::cbrt((float)n)); // k * cbrt(N)
    nd.basePos = randomInSphere(worldR);
    nd.pos     = nd.basePos; // бастапқыда базада тұрсын
    return nd;
}

Graph::Graph(int initialCount) {
    nodes.reserve(initialCount);
    for (int i = 0; i < initialCount; ++i) {
        Node nd = makeRandomNode();
        idIndex[nd.id] = nodes.size();
        nodes.push_back(nd);
    }
}

int Graph::addTask() {
    Node nd = makeRandomNode();
    idIndex[nd.id] = nodes.size();
    nodes.push_back(nd);
    return nd.id;
}

bool Graph::removeTask(int id) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return false;

    size_t idx = it->second;
    size_t last = nodes.size() - 1;

    if (idx != last) {
        std::swap(nodes[idx], nodes[last]);
        idIndex[nodes[idx].id] = idx; // swapped элементтің индексін жаңарту
    }
    nodes.pop_back();
    idIndex.erase(it);
    return true;
}

std::vector<int> Graph::ids() const {
    std::vector<int> out;
    out.reserve(idIndex.size());
    for (auto& kv : idIndex) out.push_back(kv.first);
    std::sort(out.begin(), out.end());
    return out;
}

void Graph::setNodeState(int id, NodeState s) {
    auto it = idIndex.find(id);
    if (it == idIndex.end()) return;
    nodes[it->second].state = s;
}

void Graph::update(float dt) {
    if (dt <= 0.0f) return;

    const int n = (int)nodes.size();
    if (n == 0) return;

    // ----- Дүниенің өлшемі: N артқанда әлем радиусы өседі (сығылмасын) -----
    const float worldR = std::max(1.2f, 0.28f * std::cbrt((float)n));
    const float B = worldR + 0.6f;        // куб қақпа шекарасы (±B)
    const float maxSpeed = 0.7f;          // жылдамдық шегі

    // ----- "Шарлар" геометриясы мен минималды арақашықтық -----
    const float nodeRadius = 0.07f;       // ⚠️ GraphRenderer-дегі радиуспен бірдей ұстаңыз
    const float minGap     = 0.03f;       // шарлардың арасына қосымша саңылау
    const float minDist    = 2.0f * nodeRadius + minGap;     // минималды рұқсат етілген қашықтық
    const float minDist2   = minDist * minDist;

    // Сепарация күші (жұмсақ “тығыстыру”): overlap қанша болса, сонша итереді
    const float sepK       = 10.0f;       // тебіліс қатаңдығы (керек болса реттеңіз)

    // Рандом (роуминг) және серіппе тарту параметрлері
    const float jitterScale = 0.6f;       // кездейсоқ тербеліс амплитудасы
    const float springNear  = 0.5f;       // радиус ішінде тарту
    const float springFar   = 2.0f;       // радиустан тыс тарту

    std::uniform_real_distribution<float> J(-1.0f, 1.0f);

    // ----- 1) Жұптық сепарация күшін жиналмалы түрде есептейміз -----
    std::vector<glm::vec3> sepAcc(n, glm::vec3(0.0f));

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            glm::vec3 d = nodes[i].pos - nodes[j].pos;
            float dist2 = glm::dot(d, d);
            if (dist2 > 1e-10f && dist2 < minDist2) {
                float dist = std::sqrt(dist2);
                glm::vec3 dir = d / dist;
                float overlap = (minDist - dist);      // қанша “жабысып” тұр
                // Жұмсақ тебіліс — көбірек жабысып тұрса, көбірек итереді
                glm::vec3 acc = dir * (sepK * overlap);
                sepAcc[i] += acc;
                sepAcc[j] -= acc;
            }
        }
    }

    // ----- 2) Әр түйінге барлық күштерді (acceleration) жинап, интеграция -----
    for (int i = 0; i < n; ++i) {
        auto& nd = nodes[i];

        // Кездейсоқ “жүзу” (роуминг)
        glm::vec3 a(J(rng()), J(rng()), J(rng()));
        a *= jitterScale;

        // Базалық центрге серіппелі тарту (роуминг радиусы аясында ұстау)
        glm::vec3 toC = nd.basePos - nd.pos;
        float d = glm::length(toC);
        if (d > 1e-5f) {
            glm::vec3 dir = toC / d;
            float springK = (d > nd.roamRadius) ? springFar : springNear;
            a += dir * springK;
        }

        // Күйге қарай жеңіл вертикал дрейф
        switch (nd.state) {
            case NodeState::Pending: a += glm::vec3(0.f,  0.20f, 0.f); break;
            case NodeState::Done:    a += glm::vec3(0.f,  0.35f, 0.f); break;
            case NodeState::Fail:    a += glm::vec3(0.f, -0.30f, 0.f); break;
            default: break;
        }

        // Жұптық сепарациядан келген жиналмалы күшті қосамыз
        a += sepAcc[i];

        // Интеграция
        nd.vel += a * dt;

        // Жылдамдықты шектеу
        float sp = glm::length(nd.vel);
        if (sp > maxSpeed) nd.vel = (nd.vel / sp) * maxSpeed;

        // Позиция жаңарту
        nd.pos += nd.vel * dt;

        // Глобал шекарадан шықпау (қақпалау)
        for (int ax = 0; ax < 3; ++ax) {
            float* p = (ax==0)? &nd.pos.x : (ax==1)? &nd.pos.y : &nd.pos.z;
            float* v = (ax==0)? &nd.vel.x : (ax==1)? &nd.vel.y : &nd.vel.z;
            if (*p < -B) { *p = -B; *v = std::abs(*v);  }
            if (*p >  +B) { *p =  +B; *v = -std::abs(*v); }
        }
    }
}

