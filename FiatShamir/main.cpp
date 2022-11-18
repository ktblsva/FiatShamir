#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <unistd.h>
#include <pthread.h>
#include <map>

#define MAX(x, y) (x)>(y)?(x):(y)
#define MIN(x, y) (x)<(y)?(x):(y)
#define ll long long


#define MAX_SIZE 256
#define ROUNDS 7

pthread_mutex_t mutex;
ll n;
const std::string db = "base.txt";
std::map<std::string, ll> logins;

ll modularExponentiation(ll x, ll ex, ll p) {
    ll result = 1;
    for (ll na = abs(ex); na > 0; na >>= 1) {
        if (na % 2 == 1) {
            result = (result * x) % p;
        }
        x = (x * x) % p;
    }
    return result % p;
}

ll gcd(ll a, ll b, ll *x, ll *y) {
    ll U_array[] = {MAX(a, b), 1, 0};
    ll V_array[] = {MIN(a, b), 0, 1};
    ll T_array[3];
    ll q, *swop_p, *U, *V, *T;

    q = MAX(a, b);
    if (q != a) {
        swop_p = x;
        x = y;
        y = swop_p;
    }

    U = U_array;
    V = V_array;
    T = T_array;
    while (V[0] != 0) {
        q = U[0] / V[0];
        T[0] = U[0] % V[0];
        T[1] = U[1] - q * V[1];
        T[2] = U[2] - q * V[2];
        swop_p = U;
        U = V;
        V = T;
        T = swop_p;
    }
    if (x != NULL) {
        *x = U[1];
    }
    if (y != NULL) {
        *y = U[2];
    }
    return U[0];
}

ll inversion(ll *c, ll *d, ll p) {
    ll x, y;
    ll b_c, b_d, b_p;

    do {
        *c = rand() + 1;
    } while (gcd(*c, p, &x, &y) != 1);

    b_c = *c;
    b_p = p;

    *d = x < 0 ? p + x : x;
    b_d = *d;

    return (b_c * b_d) % b_p;
}

bool ferma(ll x) {
    ll a;

    if (!(x % 2)) {
        return false;
    }
    for (int i = 0; i < 100; i++) {
        a = (rand() % (x - 2)) + 2;
        if (gcd(a, x, NULL, NULL) != 1)
            return false;
        if (modularExponentiation(a, x - 1, x) != 1)
            return false;
    }
    return true;
}

ll *eucleadian(ll a, ll b) {
    ll static u[]{a, 1, 0}, v[]{b, 0, 1}, t[]{0, 0, 0};
    ll q = 0;
    while (v[0]) {
        q = u[0] / v[0];
        t[0] = u[0] % v[0];
        t[1] = u[1] - q * v[1];
        t[2] = u[2] - q * v[2];
        u[0] = v[0];
        u[1] = v[1];
        u[2] = v[2];
        v[0] = t[0];
        v[1] = t[1];
        v[2] = t[2];

    }
    //cout << u[0] << " " << u[1] << " " << u[2] << endl;
    return u;
}

bool isPrime(ll p, int k) {

    if (p == 2) {
        return true;
    }
    if (!(p & 1)) {
        return false;
    }
    for (int i = 0; i < k; i++) {
        ll a = rand() % (p - 2) + 1;
        if (eucleadian(a, p)[0] != 1 || modularExponentiation(a, p - 1, p) != 1) {
            //cout << a << p << endl;
            return false;
        }
    }
    return true;
}

ll primeNumberRandom(ll left, ll right) {
    ll cnt = 0;
    while (1) {
        ll x = (rand() * (ll) rand() + rand()) % (right - left) + left;
        x |= 1;
        //std::cout << x << std::endl;
        if (isPrime(x, 100)) {
            return x;
        }
    }
}

void initSocket(int &serverSocket) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "socket:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }
}

void initStruct(sockaddr_in &serverStruct) {
    serverStruct.sin_family = AF_INET;
    serverStruct.sin_addr.s_addr = htonl(INADDR_ANY);
    serverStruct.sin_port = htons(0);
}

std::string getIPAndPort(const sockaddr_in &structure);

std::string getIPAndPort(const int &fd, const bool &client = true);


void *receiver(void *clientSocketPtr) {
    int clientSocket = (int) reinterpret_cast<std::intptr_t>(clientSocketPtr);
    char buff[MAX_SIZE];
    recv(clientSocket, buff, MAX_SIZE, 0);
    std::string prefix("[" + getIPAndPort(clientSocket) + "] ");
    if (buff[0] == '1') {
        std::cout << prefix << "Start registration" << std::endl;
        send(clientSocket, std::to_string(n).c_str(), MAX_SIZE, 0);
        recv(clientSocket, buff, MAX_SIZE, 0);
        std::cout << prefix << "Login " << buff << std::endl;
        std::string login(buff);
        recv(clientSocket, buff, MAX_SIZE, 0);
        std::cout << prefix << "V " << buff << std::endl;
        ll v = atoll(buff);

        logins[login] = v;
        pthread_mutex_lock(&mutex);
        std::ofstream out(db, std::ios::app);
        out << login << " " << v << std::endl;
        out.close();
        pthread_mutex_unlock(&mutex);
    } else {
        ll x, e, y;
        std::cout << prefix << "Start authentication" << std::endl;
        send(clientSocket, std::to_string(n).c_str(), MAX_SIZE, 0);
        recv(clientSocket, buff, MAX_SIZE, 0);
        std::cout << prefix << "Login " << buff << std::endl;
        std::string login(buff);
        send(clientSocket, std::to_string(ROUNDS).c_str(), MAX_SIZE, 0);
        for (int i = 0; i < ROUNDS; i++) {
            std::cout << "[INFO] Round " << i + 1 << std::endl;
            recv(clientSocket, buff, MAX_SIZE, 0);
            x = atoll(buff);
            e = rand() % 2;
            std::cout << prefix << "[DEBUG] e = " << e << std::endl;
            send(clientSocket, std::to_string(e).c_str(), MAX_SIZE, 0);
            recv(clientSocket, buff, MAX_SIZE, 0);
            y = atoll(buff);
            ll l = modularExponentiation(y, 2, n);
            ll r = (x * modularExponentiation(logins[login], e, n)) % n;
            std::cout <<prefix << "[DEBUG] L = " <<  l << ", R = " << r << std::endl;
            if (l == r) {
                send(clientSocket, std::to_string(200).c_str(), MAX_SIZE, 0);
            } else {
                send(clientSocket, std::to_string(215).c_str(), MAX_SIZE, 0);
                break;
            }
        }
    }

    close(clientSocket);
}

void *listenConnections(void *serverSocketPtr) {
    int serverSocket = (int) reinterpret_cast<std::intptr_t>(serverSocketPtr);
    listen(serverSocket, SOMAXCONN);
    int clientSocket;
    std::cout << "Listening..." << std::endl;
    while ((clientSocket = accept(serverSocket, nullptr, nullptr)) > 0) {
        pthread_t receiverThread;
        pthread_create(&receiverThread, nullptr, receiver, (void *) clientSocket);
        pthread_detach(receiverThread);
    }
    return nullptr;
}

void readDbFromFile() {
    std::ifstream file(db);
    std::string key;
    ll value;
    while (file >> key >> value) {
        logins[key] = value;
    }
    file.close();
}

int main() {
    int serverSocket;
    srand(42);
    pthread_t mTh;
    readDbFromFile();
    sockaddr_in serverStructure;
    initStruct(serverStructure);
    initSocket(serverSocket);

    ll p, q;
    p = primeNumberRandom(10000, 45000);
    q = primeNumberRandom(10000, 45000);
    n = p * q;

    std::cout << n << " " << p << " " << q << std::endl;
    if (bind(serverSocket, (sockaddr *) &serverStructure, sizeof(serverStructure)) < 0) {
        std::cerr << "bind:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server start at " + getIPAndPort(serverSocket, false) << std::endl;
    pthread_create(&mTh, nullptr, listenConnections, (void *) serverSocket);
    pthread_detach(mTh);

    std::string str;
    while (str != "stop" && str != "exit") {
        std::cin >> str;
    }
    close(serverSocket);
    pthread_mutex_destroy(&mutex);
    pthread_cancel(mTh);
    return 0;
}

std::string getIPAndPort(const sockaddr_in &structure) {
    unsigned int port = ntohs(structure.sin_port);
    std::string clientIP(inet_ntoa(structure.sin_addr));
    return clientIP + ":" + std::to_string(port);
}

std::string getIPAndPort(const int &fd, const bool &client) {
    sockaddr_in structure{};
    socklen_t size = sizeof(structure);
    if (client)
        getpeername(fd, (sockaddr *) &structure, &size);
    else
        getsockname(fd, (sockaddr *) &structure, &size);
    return getIPAndPort(structure);
}



