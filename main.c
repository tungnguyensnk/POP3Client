#include <signal.h>
#include "connect.h"
#include "crypto.h"
#include "smtp.h"
#include "pop3.h"
#include "process.h"
#include "text.h"

extern ACCOUNT account;
extern int totalLetters, totalPages;

extern void signal_handler(int signum);

SSL *sslPOP, *sslSMTP;

void clrs() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void clearScreen() {
    system("clear");
}

void printLogo() {
    clearScreen();
    printf("  __  __   _   ___ _        ___ _    ___ ___ _  _ _____ \n"
           " |  \\/  | /_\\ |_ _| |      / __| |  |_ _| __| \\| |_   _|\n"
           " | |\\/| |/ _ \\ | || |__   | (__| |__ | || _|| .` | | |  \n"
           " |_|  |_/_/ \\_|___|____|   \\___|____|___|___|_|\\_| |_|  \n\n");
}

void init() {
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, SIG_IGN);
    printLogo();
    while (sslPOP == NULL) {
        sprintf(account.username, "tungnguyensnk");
        sprintf(account.password, "heenydrjzorhurjy");
        printf(C_BOLD "Nhập tài khoản Gmail: " C_OFF);
        //fgets(account.username, sizeof(account.username), stdin);
        if (account.username[strlen(account.username) - 1] == '\n')
            account.username[strlen(account.username) - 1] = '\0';

        printf(C_BOLD"Nhập mật khẩu Gmail: "C_OFF);
        //fgets(account.password, sizeof(account.password), stdin);
        if (account.password[strlen(account.password) - 1] == '\n')
            account.password[strlen(account.password) - 1] = '\0';

        sslPOP = verifyAccount(account);
        if (sslPOP == NULL) {
            printLogo();
            printf(C_RED"Sai tài khoản hoặc mật khẩu.\n"C_OFF);
        }
    }
    sslSMTP = loginSMTPServer(account);
    clearScreen();

}

//void xemChiTietThu(int id) {
//    printLogo();
//    EMAIL *email = getContent(sslPOP, id);
//    printf("Tiêu đề: %s\nNgười gửi: %s\nNgười Nhận: %s\nNội dung:\n%s\n", email->subject, email->from, email->to,
//           email->content);
//    genHTML(email->html);
//    while (1) {
//        int hanhdong = -1;
//        printf("\n\t0. Trở lại menu trước\n\t1. Mở thư định dạng HTML\n\t2. Trả lời mail%s\nChọn 1 hành động: ",
//               email->filename != NULL ? "\n\t3. Mở file đính kèm" : "");
//        scanf("%d", &hanhdong);
//        clrs();
//        switch (hanhdong) {
//            case 0:
//                return;
//            case 1:
//                system("/mnt/c/Windows/System32/cmd.exe /c \"start msedge file:///%cd%/content.html\"");
//                break;
//            case 2:
//                break;
//            case 3:
//                if(email->filename != NULL){
//                    char command[100] = {0};
//                    sprintf(command, "/mnt/c/Windows/System32/cmd.exe /c \"start msedge file:///%%cd%%/%s\"", email->filename);
//                    system(command);
//                    break;
//                }
//            default: {
//                printf("Ban da nhap sai. Nhap 1 so tu 1 den 3: ");
//                continue;
//            }
//        }
//    }
//}

void menuChonThu(int page) {
    while (0 == 0) {
        printLogo();
        showPageLetters(sslPOP, page);
        printf("Chọn mail để xem chi tiết (Nhập 0 để trở lại menu trước): ");
        int id = -1;
        while (id > (page * 10 < totalLetters ? page * 10 : totalLetters) || id < (page - 1) * 10 + 1) {
            scanf("%d", &id);
            clrs();
            if (id == 0)
                return;
            if (id > (page * 10 < totalLetters ? page * 10 : totalLetters) || id < (page - 1) * 10 + 1)
                printf("Nhập 1 số từ %d đến %d: ", page * 10 < totalLetters ? page * 10 : totalLetters,
                       id < (page - 1) * 10 + 1);
        }
        //xemChiTietThu(id);
    }
}

void menuXemDSThu() {
    while (0 == 0) {
        printLogo();
        printf("Có %d trang (Nhập 0 để trở lại menu trước).\nChọn trang: ", totalPages);
        int page = -1;
        while (page > totalPages || page < 1) {
            scanf("%d", &page);
            clrs();
            if (page == -0)
                return;
            if (page < 1 || page > totalPages)
                printf("Nhập 1 số từ 0 đến %d: ", totalPages);
        }
        menuChonThu(page);
    }
}

void menuGuiThu() {

}

void menu() {
    printLogo();
    getTotalLetters(sslPOP);
    printf("Có %d tin nhắn trong hộp thư.\n", totalLetters);
    int hanhdong = 0;
    printf("\t1.Xem hộp thư đến\n\t2.Gửi thư mới\n\t3.Thoát\nChọn hành động: ");
    scanf("%d", &hanhdong);
    while (1) {
        switch (hanhdong) {
            case 1:
                menuXemDSThu();
                return;
            case 2:
                menuGuiThu();
                return;
            case 3:
                printf("Cảm ơn bạn đã sử dụng dịch vụ.\n");
                exit(0);
            default: {
                printf("Ban da nhap sai. Nhap 1 so tu 1 den 3: ");
                clrs();
                scanf("%d", &hanhdong);
            }
        }
    }
}

int main() {
    init();
    //analyzingHeaderMail(sslPOP,1);
    //analyzingHeaderMail(sslPOP,2);
    //analyzingHeaderMail(sslPOP,7);
//    EMAIL *email = getContent(sslPOP, 2);
//    printf("Tiêu đề: %s\nNgười gửi: %s\nNgười Nhận: %s\nNội dung:\n%s\n", email->subject, email->from, email->to,
//           email->content);
//    genHTML(email->html);
//    while (0 == 0)
//        menu();

    getTotalLetters(sslPOP);
    showPageLetters(sslPOP, 1);
    //genHTML(getContent(ssl, 5)->html);
    free(sslPOP);
    free(sslSMTP);
    return 0;
}