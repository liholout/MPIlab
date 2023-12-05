#include <iostream>
#include <mpi.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <random>


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Повар
    if (world_rank == 0) {
        std::vector<std::string> menu = {"Паста", "Пицца", "Суп", "Салат", "Стейк"};
        int students = 15;

        for (int i = 0; i < students * 2; ++i) {
            MPI_Request request;
            MPI_Status status;
            int dish;

            // Принимаем заказ от студента
            MPI_Irecv(&dish, 1, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            std::cout << "Повар принял заказ от студента " << status.MPI_SOURCE << ": " << menu[dish] << std::endl;

            // Готовим блюдо
            usleep(100000); // Имитация приготовления блюда

            // Выдаем блюдо студенту
            MPI_Isend(&dish, 1, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            std::cout << "Повар выдал блюдо студенту " << status.MPI_SOURCE << ": " << menu[dish] << std::endl;
        }
    }
    // Студенты
    else {
        for (int i = 0; i < 2; ++i) {
            std::mt19937 rng(std::random_device{}()); 
            srand(static_cast<unsigned int>(time(nullptr)));
            MPI_Request request;
            std::vector<std::string> menu = {"Паста", "Пицца", "Суп", "Салат", "Стейк"};
            char random_dish[] = menu[rng() % menu.size()];
            char* p = &random_dish;
            
            // Отправляем заказ повару
            MPI_Isend(&random_dish, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            std::cout << "Студент " << world_rank << " отправил заказ повару: " << menu[random_dish] << std::endl;


            // Принимаем блюдо от повара
            MPI_Irecv(&random_dish, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            std::cout << "Студент " << world_rank << " получил блюдо от повара: " << menu[random_dish] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
