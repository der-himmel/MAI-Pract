#include <iostream>
#include <math.h>
#include <vector>
#include <queue>  
#include <random>
#include <cfloat>
#include <locale>

using namespace std;


int random(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

double random(double min, double max)
{
    static bool initialized = false;
    if (!initialized)
    {
        srand(time(NULL));
        initialized = true;
    }
    double range = max - min;
    double fraction = (double)rand() / RAND_MAX;
    return min + (fraction * range);
}

struct Request
{
    int prior;
    double arrival_time;
};

double without_prior_exprnt(int N, double lambda, double load, double c1, double c2, double c3, double c4)
{
    queue <Request> queue;
    random_device rd;
    mt19937 gen(rd());
    vector <double> wait_time(N);

    double arr_wait_time;
    double sum_lambda = 4.0 * lambda; // так как потоки простейшие, то объединяем в один суммарный поток
    exponential_distribution <double> req_distribution(sum_lambda); // интервалы между заявками распределены по экспоненте
    exponential_distribution <double> service_distribution(lambda / load); // длительность обслуживания распределена по экспоненте

    // генерация заявок
    double temp_arr_time = 0;
    for (int i = 0; i < N; ++i)
    {
        Request request;
        request.arrival_time = temp_arr_time;

        temp_arr_time += req_distribution(gen);
        queue.push(request);
        //cout << queue.back().arrival_time << ' ';
    }

    //cout << endl << endl;
    //vector <double> vec_service(N);

    // обработка заявок
    double delay_time = 0, current_time = 0, downtime = 0;
    for (int i = 0; i < N; ++i)
    {

        if (current_time > queue.front().arrival_time)
        {
            delay_time = current_time - queue.front().arrival_time;
            downtime = 0;
        }
        else {
            downtime = queue.front().arrival_time - current_time;
            delay_time = 0;
        }

        double temp = service_distribution(gen); // вместо (load / lambda) = ср. врем. обработки можно поставить service_distribution(generator2)

        /*vec_service[i] = temp;
        cout << temp << ' ';
        */

        wait_time[i] = delay_time;
        current_time += temp + downtime;
        queue.pop();

    }


    /*double sum_ = 0;
    for (int i = 0; i < N; ++i) {
        sum_serv += vec_service[i];
    }

    cout << endl << "sum_service " << sum_serv / ( N - 0) << endl;

    */
    // поиск среднего времени ожидания
    double sum = 0;
    for (int i = 0; i < N; ++i)
    {
        sum += wait_time[i];
    }

    arr_wait_time = sum / (N);

    return arr_wait_time;

}

void without_prior_theor(double lambda, double load, double c1, double c2, double c3, double c4)
{
    double ro = lambda / load;
    double w = 4 * lambda / ((lambda / load) * ((lambda / load) - 4 * lambda));
    cout << "Среднее время ожидания для бесприоритетной дисциплины (теоретич.) - " << w << " сек\n";
    cout << "Общие временные потери (теоретич.) - " << lambda * w * (c1 + c2 + c3 + c4) << " сек\n";
}

void otnosit_prior_exprnt(int N, double lambda, double load, double c1, double c2, double c3, double c4)
{
    queue <Request> q1, q2, q3, q4;
    random_device rd;
    mt19937 gen(rd());
    vector <double> wait_time_k1, wait_time_k2, wait_time_k3, wait_time_k4;

    double w1, w2, w3, w4;
    double sum_lambda = 4.0 * lambda; // так как потоки простейшие, то объединяем в один суммарный поток
    exponential_distribution <double> req_distribution(sum_lambda); // интервалы между заявками распределены по экспоненте
    exponential_distribution <double> service_distribution(lambda / load); // длительность обслуживания распределена по экспоненте

    // генерация заявок
    double temp_arr_time = 0;
    for (int i = 0; i < N; ++i)
    {
        Request request;
        request.prior = random(1, 4);
        request.arrival_time = temp_arr_time;

        temp_arr_time += req_distribution(gen);
        switch (request.prior) {
        case 1:
            q1.push(request);
            break;
        case 2:
            q2.push(request);
            break;
        case 3:
            q3.push(request);
            break;
        case 4:
            q4.push(request);
            break;
        default:
            break;
        }

    }


    // обработка заявок
    int i = 0;
    double delay_time = 0, current_time = 0;
    while (i != N)
    {
        double downtime = DBL_MAX;
        // анализируем, пришло ли что-нибудь в какой-нибудь очереди, начиная с 1 приоритета
        if (!q1.empty() && current_time >= q1.front().arrival_time)
        {
            delay_time = current_time - q1.front().arrival_time;
            wait_time_k1.push_back(delay_time);
            q1.pop();
        }
        else {
            if (!q2.empty() && current_time >= q2.front().arrival_time)
            {
                delay_time = current_time - q2.front().arrival_time;
                wait_time_k2.push_back(delay_time);
                current_time += service_distribution(gen);
                q2.pop();
            }
            else {
                if (!q3.empty() && current_time >= q3.front().arrival_time)
                {
                    delay_time = current_time - q3.front().arrival_time;
                    wait_time_k3.push_back(delay_time);
                    current_time += service_distribution(gen);
                    q3.pop();
                }
                else {
                    if (!q4.empty() && current_time >= q4.front().arrival_time)
                    {
                        delay_time = current_time - q4.front().arrival_time;
                        wait_time_k4.push_back(delay_time);
                        current_time += service_distribution(gen);
                        q4.pop();
                    }
                    else { // если ничего не успело прийти, то СМО простаивает и анализирует, что к ней быстрее придет
                        if (!q1.empty() && q1.front().arrival_time - current_time < downtime)
                        {
                            downtime = q1.front().arrival_time - current_time;
                            wait_time_k1.push_back(0);
                            current_time += service_distribution(gen) + downtime;
                            q1.pop();
                        }
                        else {
                            if (!q2.empty() && q2.front().arrival_time - current_time < downtime)
                            {
                                downtime = q2.front().arrival_time - current_time;
                                wait_time_k2.push_back(0);
                                current_time += service_distribution(gen) + downtime;
                                q2.pop();
                            }
                            else {
                                if (!q3.empty() && q3.front().arrival_time - current_time < downtime)
                                {
                                    downtime = q3.front().arrival_time - current_time;
                                    wait_time_k3.push_back(0);
                                    current_time += service_distribution(gen) + downtime;
                                    q3.pop();
                                }
                                else {
                                    if (!q4.empty() && q4.front().arrival_time - current_time < downtime)
                                    {
                                        downtime = q4.front().arrival_time - current_time;
                                        wait_time_k4.push_back(0);
                                        current_time += service_distribution(gen) + downtime;
                                        q4.pop();
                                    }
                                }

                            }

                        }

                    }
                }
            }
        }

        ++i;

    }

    double sum = 0;
    for (int i = 0; i < wait_time_k1.size(); ++i) {
        sum += wait_time_k1[i];
    }
    w1 = sum / wait_time_k1.size();
    cout << "\nСреднее время ожидания для заявок 1 класса (эксп.) - " << w1 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k2.size(); ++i) {
        sum += wait_time_k2[i];
    }
    w2 = sum / wait_time_k2.size();
    cout << "Среднее время ожидания для заявок 2 класса (эксп.) - " << w2 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k3.size(); ++i) {
        sum += wait_time_k3[i];
    }
    w3 = sum / wait_time_k3.size();
    cout << "Среднее время ожидания для заявок 3 класса (эксп.) - " << w3 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k4.size(); ++i) {
        sum += wait_time_k4[i];
    }
    w4 = sum / wait_time_k4.size();
    cout << "Среднее время ожидания для заявок 4 класса (эксп.) - " << w4 << " сек\n";


    cout << "Общие временные потери (эксп.) - " << c1 * lambda * w1 + c2 * lambda * w2 + c3 * lambda * w3 + c4 * lambda * w4 << " сек\n";

}

void otnosit_prior_theory(double lambda, double load, double c1, double c2, double c3, double c4)
{
    double w1, w2, w3, w4;
    w1 = 4 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 0 * load) * (1 - 1 * load));
    w2 = 4 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 1 * load) * (1 - 2 * load));
    w3 = 4 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 2 * load) * (1 - 3 * load));
    w4 = 4 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 3 * load) * (1 - 4 * load));
    cout << "\nСреднее время ожидания для заявок 1 класса (теоретич.) - " << w1 << " сек\n";
    cout << "Среднее время ожидания для заявок 2 класса (теоретич.) - " << w2 << " сек\n";
    cout << "Среднее время ожидания для заявок 3 класса (теоретич.) - " << w3 << " сек\n";
    cout << "Среднее время ожидания для заявок 4 класса (теоретич.) - " << w4 << " сек\n";
    cout << "Общие временные потери (теоретич.) - " << c1 * lambda * w1 + c2 * lambda * w2 + c3 * lambda * w3 + c4 * lambda * w4 << " сек\n";
}

void abs_prior_exprnt(int N, double lambda, double load, double c1, double c2, double c3, double c4)
{
    queue <Request> q1, q2, q3, q4;
    random_device rd;
    mt19937 gen(rd());
    vector <double> wait_time_k1, wait_time_k2, wait_time_k3, wait_time_k4;

    double w1, w2, w3, w4;
    double sum_lambda = 4.0 * lambda; // так как потоки простейшие, то объединяем в один суммарный поток
    exponential_distribution <double> req_distribution(sum_lambda); // интервалы между заявками распределены по экспоненте
    exponential_distribution <double> service_distribution(lambda / load); // длительность обслуживания распределена по экспоненте

    // генерация заявок
    double temp_arr_time = 0;
    for (int i = 0; i < N; ++i)
    {
        Request request;
        request.prior = random(1, 4);
        request.arrival_time = temp_arr_time;

        temp_arr_time += req_distribution(gen);
        switch (request.prior) {
        case 1:
            q1.push(request);
            break;
        case 2:
            q2.push(request);
            break;
        case 3:
            q3.push(request);
            break;
        case 4:
            q4.push(request);
            break;
        default:
            break;
        }
    }


    // обработка заявок
    int i = 0, prior_procc_req = 0; // номер приоритета обслуживаемой заявки
    double delay_time = 0, current_time = 0;
    while (i != N)
    {
        double downtime = DBL_MAX;
        // анализируем, пришло ли что-нибудь в какой-нибудь очереди, начиная с 1 приоритета
        if (!q1.empty() && current_time >= q1.front().arrival_time)
        {
            delay_time = current_time - q1.front().arrival_time;
            wait_time_k1.push_back(delay_time);
            q1.pop();
        }
        else {
            if (!q2.empty() && current_time >= q2.front().arrival_time)
            {
                delay_time = current_time - q2.front().arrival_time;
                current_time += service_distribution(gen);
                if (!q1.empty() && current_time > q1.front().arrival_time) {
                    double temp = service_distribution(gen);
                    current_time += temp;
                    delay_time += temp;
                    ++i;
                    wait_time_k1.push_back(0);
                    q1.pop();
                }
                wait_time_k2.push_back(delay_time);
                q2.pop();
            }
            else {
                if (!q3.empty() && current_time >= q3.front().arrival_time)
                {
                    delay_time = current_time - q3.front().arrival_time;
                    current_time += service_distribution(gen);
                    if (!q1.empty() && current_time > q1.front().arrival_time) {
                        double temp = service_distribution(gen);
                        delay_time += temp;
                        current_time += temp;
                        ++i;
                        wait_time_k1.push_back(0);
                        q1.pop();
                    }
                    if (!q2.empty() && current_time > q2.front().arrival_time) {
                        double temp1 = service_distribution(gen);
                        delay_time += temp1;
                        current_time += temp1;

                        if (!q1.empty() && current_time > q1.front().arrival_time) {
                            double temp2 = service_distribution(gen);
                            delay_time += temp2;
                            current_time += temp2;
                            ++i;
                            wait_time_k1.push_back(0);
                            wait_time_k2.push_back(temp2);
                            q1.pop();
                        }
                        else {
                            wait_time_k2.push_back(0);
                        }

                        ++i;
                        q2.pop();
                    }
                    wait_time_k3.push_back(delay_time);
                    q3.pop();
                }
                else {
                    if (!q4.empty() && current_time >= q4.front().arrival_time)
                    {
                        delay_time = current_time - q4.front().arrival_time;
                        current_time += service_distribution(gen);
                        if (!q1.empty() && current_time > q1.front().arrival_time) {
                            double temp = service_distribution(gen);
                            delay_time += temp;
                            current_time += temp;
                            ++i;
                            wait_time_k1.push_back(0);
                            q1.pop();
                        }
                        if (!q2.empty() && current_time > q2.front().arrival_time) {
                            double temp1 = service_distribution(gen);
                            delay_time += temp1;
                            current_time += temp1;
                            if (!q1.empty() && current_time > q1.front().arrival_time) {
                                double temp2 = service_distribution(gen);
                                delay_time += temp2;
                                current_time += temp2;
                                ++i;
                                wait_time_k1.push_back(0);
                                wait_time_k2.push_back(temp2);
                                q1.pop();
                            }
                            else {
                                wait_time_k2.push_back(0);
                            }
                            ++i;
                            q2.pop();
                        }
                        if (!q3.empty() && current_time > q3.front().arrival_time) {
                            double temp1 = service_distribution(gen);
                            delay_time += temp1;
                            current_time += temp1;
                            double local_delay_time = 0;
                            if (!q1.empty() && current_time > q1.front().arrival_time) {
                                double temp2 = service_distribution(gen);
                                delay_time += temp2;
                                current_time += temp2;
                                ++i;
                                wait_time_k1.push_back(0);
                                local_delay_time += temp2;
                                q1.pop();
                            }
                            if (!q2.empty() && current_time > q2.front().arrival_time) {
                                double temp1 = service_distribution(gen);
                                delay_time += temp1;
                                current_time += temp1;
                                local_delay_time += temp1;
                                if (!q1.empty() && current_time > q1.front().arrival_time) {
                                    double temp2 = service_distribution(gen);
                                    delay_time += temp2;
                                    current_time += temp2;
                                    local_delay_time += temp2;
                                    ++i;
                                    wait_time_k1.push_back(0);
                                    wait_time_k2.push_back(temp2);
                                    q1.pop();
                                }
                                else {
                                    wait_time_k2.push_back(0);
                                }
                                ++i;
                                q2.pop();
                            }
                            wait_time_k3.push_back(local_delay_time);
                            ++i;
                            q3.pop();
                        }
                        wait_time_k4.push_back(delay_time);
                        q4.pop();
                    }
                    else { // если ничего не успело прийти, то СМО простаивает и анализирует, что к ней быстрее придет
                        if (!q1.empty() && q1.front().arrival_time - current_time < downtime)
                        {
                            downtime = q1.front().arrival_time - current_time;
                            wait_time_k1.push_back(0);
                            current_time += service_distribution(gen) + downtime;
                            q1.pop();
                        }
                        else {
                            if (!q2.empty() && q2.front().arrival_time - current_time < downtime)
                            {
                                downtime = q2.front().arrival_time - current_time;
                                current_time += service_distribution(gen) + downtime;
                                if (!q1.empty() && current_time > q1.front().arrival_time) {
                                    double temp2 = service_distribution(gen);
                                    current_time += temp2;
                                    ++i;
                                    wait_time_k1.push_back(0);
                                    wait_time_k2.push_back(temp2);
                                    q1.pop();
                                }
                                else {
                                    wait_time_k2.push_back(0);
                                }

                                q2.pop();
                            }
                            else {
                                if (!q3.empty() && q3.front().arrival_time - current_time < downtime)
                                {
                                    downtime = q3.front().arrival_time - current_time;
                                    current_time += service_distribution(gen) + downtime;
                                    double local_delay_time = 0;
                                    if (!q1.empty() && current_time > q1.front().arrival_time) {
                                        double temp = service_distribution(gen);
                                        current_time += temp;
                                        ++i;
                                        wait_time_k1.push_back(0);
                                        local_delay_time += temp;
                                        q1.pop();
                                    }
                                    if (!q2.empty() && current_time > q2.front().arrival_time) {
                                        double temp1 = service_distribution(gen);
                                        local_delay_time += temp1;
                                        current_time += temp1;

                                        if (!q1.empty() && current_time > q1.front().arrival_time) {
                                            double temp2 = service_distribution(gen);
                                            local_delay_time += temp2;
                                            current_time += temp2;
                                            ++i;
                                            wait_time_k1.push_back(0);
                                            wait_time_k2.push_back(temp2);
                                            q1.pop();
                                        }
                                        else {
                                            wait_time_k2.push_back(0);
                                        }

                                        ++i;
                                        q2.pop();
                                    }
                                    wait_time_k3.push_back(local_delay_time);
                                    q3.pop();
                                }
                                else {
                                    if (!q4.empty() && q4.front().arrival_time - current_time < downtime)
                                    {
                                        downtime = q4.front().arrival_time - current_time;
                                        current_time += service_distribution(gen) + downtime;
                                        double local_delay_time = 0;
                                        if (!q1.empty() && current_time > q1.front().arrival_time) {
                                            double temp = service_distribution(gen);
                                            local_delay_time += temp;
                                            current_time += temp;
                                            ++i;
                                            wait_time_k1.push_back(0);
                                            q1.pop();
                                        }
                                        if (!q2.empty() && current_time > q2.front().arrival_time) {
                                            double temp1 = service_distribution(gen);
                                            local_delay_time += temp1;
                                            current_time += temp1;
                                            if (!q1.empty() && current_time > q1.front().arrival_time) {
                                                double temp2 = service_distribution(gen);
                                                local_delay_time += temp2;
                                                current_time += temp2;
                                                ++i;
                                                wait_time_k1.push_back(0);
                                                wait_time_k2.push_back(temp2);
                                                q1.pop();
                                            }
                                            else {
                                                wait_time_k2.push_back(0);
                                            }
                                            ++i;
                                            q2.pop();
                                        }
                                        if (!q3.empty() && current_time > q3.front().arrival_time) {
                                            double temp1 = service_distribution(gen);
                                            delay_time += temp1;
                                            current_time += temp1;
                                            double local_delay_time1 = 0;
                                            if (!q1.empty() && current_time > q1.front().arrival_time) {
                                                double temp2 = service_distribution(gen);
                                                local_delay_time += temp2;
                                                current_time += temp2;
                                                ++i;
                                                wait_time_k1.push_back(0);
                                                local_delay_time1 += temp2;
                                                q1.pop();
                                            }
                                            if (!q2.empty() && current_time > q2.front().arrival_time) {
                                                double temp1 = service_distribution(gen);
                                                local_delay_time += temp1;
                                                current_time += temp1;
                                                local_delay_time1 += temp1;
                                                if (!q1.empty() && current_time > q1.front().arrival_time) {
                                                    double temp2 = service_distribution(gen);
                                                    local_delay_time += temp2;
                                                    current_time += temp2;
                                                    local_delay_time1 += temp2;
                                                    ++i;
                                                    wait_time_k1.push_back(0);
                                                    wait_time_k2.push_back(temp2);
                                                    q1.pop();
                                                }
                                                else {
                                                    wait_time_k2.push_back(0);
                                                }
                                                ++i;
                                                q2.pop();
                                            }
                                            wait_time_k3.push_back(local_delay_time1);
                                            ++i;
                                            q3.pop();
                                        }
                                        wait_time_k4.push_back(local_delay_time);
                                        q4.pop();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ++i;
    }

    double sum = 0;
    for (int i = 0; i < wait_time_k1.size(); ++i) {
        sum += wait_time_k1[i];
    }
    w1 = sum / wait_time_k1.size();
    cout << "\nСреднее время ожидания для заявок 1 класса (эксп.) - " << w1 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k2.size(); ++i) {
        sum += wait_time_k2[i];
    }
    w2 = sum / wait_time_k2.size();
    cout << "Среднее время ожидания для заявок 2 класса (эксп.) - " << w2 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k3.size(); ++i) {
        sum += wait_time_k3[i];
    }
    w3 = sum / wait_time_k3.size();
    cout << "Среднее время ожидания для заявок 3 класса (эксп.) - " << w3 << " сек\n";

    sum = 0;
    for (int i = 0; i < wait_time_k4.size(); ++i) {
        sum += wait_time_k4[i];
    }
    w4 = sum / wait_time_k4.size();
    cout << "Среднее время ожидания для заявок 4 класса (эксп.) - " << w4 << " сек\n";


    cout << "Общие временные потери (эксп.) - " << c1 * lambda * w1 + c2 * lambda * w2 + c3 * lambda * w3 + c4 * lambda * w4 << " сек\n";

}

void abs_prior_theory(double lambda, double load, double c1, double c2, double c3, double c4)
{
    double w1, w2, w3, w4;
    w1 = 1 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 0 * load) * (1 - 1 * load)) + (0 * load) * (load / lambda) / (1 - (0 * load));
    w2 = 2 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 1 * load) * (1 - 2 * load)) + (1 * load) * (load / lambda) / (1 - (1 * load));
    w3 = 3 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 2 * load) * (1 - 3 * load)) + (2 * load) * (load / lambda) / (1 - (2 * load));
    w4 = 4 * lambda * pow((load / lambda), 2) * 2 / (2 * (1 - 3 * load) * (1 - 4 * load)) + (3 * load) * (load / lambda) / (1 - (3 * load));
    cout << "\nСреднее время ожидания для заявок 1 класса (теоретич.) - " << w1 << " сек\n";
    cout << "Среднее время ожидания для заявок 2 класса (теоретич.) - " << w2 << " сек\n";
    cout << "Среднее время ожидания для заявок 3 класса (теоретич.) - " << w3 << " сек\n";
    cout << "Среднее время ожидания для заявок 4 класса (теоретич.) - " << w4 << " сек\n";
    cout << "Общие временные потери (теоретич.) - " << c1 * lambda * w1 + c2 * lambda * w2 + c3 * lambda * w3 + c4 * lambda * w4 << " сек\n";

}

int main()
{
    setlocale(LC_ALL, "Russian_Russia.1251");

    double lambda = 0.5, load = 0.221, c1 = 1, c2 = 2, c3 = 3, c4 = 4; // исходные данные
    int N = 10000; // количество заявок

    cout << "\t\t\tБесприоритетная дисциплина\n";
    cout << "Среднее время ожидания для бесприоритетной дисциплины (экспер.) - " << without_prior_exprnt(N, lambda, load, c1, c2, c3, c4) << " сек\n";
    cout << "Общие временные потери (экспер.) - " << lambda * (c1 + c2 + c3 + c4) * without_prior_exprnt(N, lambda, load, c1, c2, c3, c4) << " сек\n";
    without_prior_theor(lambda, load, c1, c2, c3, c4);

    cout << "\n\t\t\tДисциплина с относительным приоритетом\n";
    otnosit_prior_exprnt(N, lambda, load, c1, c2, c3, c4);
    otnosit_prior_theory(lambda, load, c1, c2, c3, c4);

    cout << "\n\t\t\tДисциплина с абсолютным приоритетом\n";
    abs_prior_exprnt(N, lambda, load, c1, c2, c3, c4);
    abs_prior_theory(lambda, load, c1, c2, c3, c4);

}
