#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"
#include <vector>
#include <algorithm>


#define MAX(x, y) (x)>(y)?(x):(y)
#define MIN(x, y) (x)<(y)?(x):(y)

#define MAX_SIZE 256
#define ll long long
using namespace rapidjson;

int setServerPort();

std::string setServerIP();

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

ll coprime(ll n) {
    ll s, x, y;
    s = (rand() * rand() + rand()) % n;
    while (gcd(s, n, 0, 0) != 1) {
        s--;
    }
    if (s < 0) {
        s *= -1;
    }
    return s;
}

void initSocket(int &clientSocket) {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "socket:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }
}

void initStruct(sockaddr_in &serverStruct) {
    serverStruct.sin_family = AF_INET;
    serverStruct.sin_addr.s_addr = inet_addr(setServerIP().c_str());
    serverStruct.sin_port = htons(setServerPort());
}

std::vector<std::string> possible_logins;
const std::string logins = "logins.txt";

void readDbFromFile() {
    std::ifstream file(logins);
    std::string key;
    while (file >> key) {
        possible_logins.push_back(key);
    }
    file.close();
}

int main() {
    srand(42);
    int clientSocket;
    sockaddr_in server;
    initStruct(server);
    char buff[MAX_SIZE];
    int choose;
    readDbFromFile();
    while (1) {
        std::cout << "Enter 1 - for registration, 2 - for authentication, 0 for exit \n>";
        std::cin >> choose;
        initSocket(clientSocket);
        if (connect(clientSocket, (struct sockaddr *) &server, sizeof(server)) < 0) {
            std::cerr << "connect:" << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        ll s, v, n;
        if (choose == 1) {
            //сообщаем серверу, что будет регистрация проходить
            send(clientSocket, std::to_string(choose).c_str(), MAX_SIZE, MSG_NOSIGNAL);
            //получаем n
            recv(clientSocket, buff, MAX_SIZE, MSG_NOSIGNAL);
            n = atoll(buff);
            s = coprime(n);
            v = modularExponentiation(s, 2, n);

            std::cout << "[DEBUG] n =" << n << ", s = " << s << ", v = " << v << std::endl;
            std::cout << "Enter your login: ";
            std::string login;
            std::cin >> login;
            //отправляем логин и открытый ключ v
            send(clientSocket, login.c_str(), MAX_SIZE, MSG_NOSIGNAL);
            send(clientSocket, std::to_string(v).c_str(), MAX_SIZE, MSG_NOSIGNAL);

            //менеджер паролей :D
//            Document d;
//            rapidjson::Document::AllocatorType &allocator = d.GetAllocator();
//            d.SetObject();
//            d.AddMember("v", (int64_t) v, allocator);
//            d.AddMember("s", (int64_t) s, allocator);
//            std::ofstream fout(login + ".json", std::ios::binary);
//            OStreamWrapper out(fout);
//            Writer<OStreamWrapper> writer(out);
//            d.Accept(writer);
//            fout.close();
            close(clientSocket);

            std::ofstream loginsout(logins, std::ios::app);
            loginsout << login << std::endl;
            loginsout.close();
            possible_logins.push_back(login);

        } else if (choose == 2) {
            ll x, r, y;
            std::string login;
            int e;
            std::cout << "Enter your login: ";
            std::cin >> login;
            if (std::find(possible_logins.begin(), possible_logins.end(), login) == possible_logins.end()) {
                std::cout << "[WARN] You should register your profile first!" << std::endl;
                continue;
            }
            //сообщаем серверу о том, что сейчас будет проходить аутентификация
            send(clientSocket, std::to_string(choose).c_str(), MAX_SIZE, MSG_NOSIGNAL);
            //получаем n
            recv(clientSocket, buff, MAX_SIZE, MSG_NOSIGNAL);
            n = atoll(buff);

            //так делать нельзя, но чтобы было легче проверять
//            std::ifstream ifs(login + ".json");
//            IStreamWrapper isw{ifs};
//            Document doc{};
//            doc.ParseStream(isw);
//            StringBuffer buffer{};
//            Writer<StringBuffer> writer{buffer};
//            doc.Accept(writer);
//            ll s = doc["s"].GetInt64();
//            ifs.close();

//            std::cout << "[DEBUG] Your s is " << s << std::endl;
            std::cout << "Enter your s: ";
            std::cin >> s;

            //отправляем логин
            send(clientSocket, login.c_str(), MAX_SIZE, MSG_NOSIGNAL);
            //получаем кол-во раундов
            recv(clientSocket, buff, MAX_SIZE, MSG_NOSIGNAL);
            int rounds = atoi(buff);
            for (int i = 0; i < rounds; i++) {
                std::cout << "[INFO] Round " << i + 1 << std::endl;
                r = rand() % n + 1;
                x = modularExponentiation(r, 2, n);
                //отправляем х
                send(clientSocket, std::to_string(x).c_str(), MAX_SIZE, MSG_NOSIGNAL);
                //получаем е
                recv(clientSocket, buff, MAX_SIZE, MSG_NOSIGNAL);
                e = atoi(buff);
                y = (r * modularExponentiation(s, e, n)) % n;
                std::cout << "[DEBUG] x = " << x << " y = " << y << std::endl;
                send(clientSocket, std::to_string(y).c_str(), MAX_SIZE, MSG_NOSIGNAL);
                recv(clientSocket, buff, MAX_SIZE, MSG_NOSIGNAL);
                std::cout << "[INFO] Round is over with code " << buff << std::endl;
                int code = atoi(buff);
                if (code != 200) {
                    std::cout << "[INFO] Authentication FAILED!" << std::endl;
                    break;
                }
            }
            std::cout << "[INFO] Authentication PASSED!" << std::endl;
            close(clientSocket);
        } else {
            break;
        }
    }
    return 0;
}

std::string setServerIP() {
    std::string ip;
    unsigned char buf[sizeof(struct in6_addr)];
    do {
        std::cout << "Enter IPv4 server address " << std::endl;
        std::cout << ">";
        getline(std::cin, ip);
    } while (inet_pton(AF_INET, ip.c_str(), buf) != 1);
    return ip;
}

int setServerPort() {
    int port;
    do {
        std::cout << "Enter a port value." << std::endl;
        std::cout << ">";
        std::cin >> port;
    } while (port <= 0 || port > 65535);
    return port;
}






