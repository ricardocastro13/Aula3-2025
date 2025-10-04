cmake_minimum_required(VERSION 3.25)
project(scheduling C)

set(CMAKE_C_STANDARD 11)

# O scheduler precisa de todos os ficheiros do escalonador
add_executable(scheduler ossim.c queue.c fifo.c sjf.c RR.h)

# As aplicações não precisam do SJF
add_executable(app app.c)

add_executable(app-io app-io.c burst_queue.c)