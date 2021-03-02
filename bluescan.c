#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>          // заголовок, определяющий семейство интернет-протоколов
#include <bluetooth/bluetooth.h> // загoловки необходимые
#include <bluetooth/hci.h>       // для работы с семейством
#include <bluetooth/hci_lib.h>   // протоколов BlueZ

int main(int argc, char **argv)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL); //передаем NULL в hci_get_route, чтобы извлечь номер ресурса первого доступного адаптера Bluetooth
    sock = hci_open_dev( dev_id ); // функция hci_open_dev открывает сокет Bluetooth c указанным номером ресурса
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8; //длительность запроса hci_inquiry. В данном случае не более 1.28 * 8 сек.
    max_rsp = 255; //задаем максимальное колличество устройств дл функции hci_inquiry
    flags = IREQ_CACHE_FLUSH; // флаг для функции hci_inquiry, очищающий кэш ранее обнаруженных устройств  перед выполнением текущего запроса
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags); //функция hci_inquiry выполняет обнаружение устройств Bluetooth и возвращает список обнаруженных устройств и некоторую базовую информацию о них в переменной ii
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr); //bdaddr_t - Базовая структура данных, используемая для указания адреса устройства Bluetooth
        memset(name, 0, sizeof(name)); // int ba2str - функция для преобразования между строками и структурами bdaddr_t. она берет 6-байтовое число из bdaddr_t и распаковывает его в 48 битный адрес. 
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), // функция hci_read_remote_name определяет удобные для пользователя имена, связанные с этими адресами, и представляет их пользователю
            name, 0) < 0)
        strcpy(name, "[unknown]");
        printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock );
    return 0;
}
