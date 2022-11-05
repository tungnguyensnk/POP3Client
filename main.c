#include "connect.h"
#include "crypto.h"
#include "smtp.h"
#include "pop3.h"

extern ACCOUNT account;
extern int totalLetters, totalPages;
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
    printLogo();
    while (sslPOP == NULL) {
        sprintf(account.username, "tungnguyensnk");
        sprintf(account.password, "heenydrjzorhurjy");
        printf("Nhập tài khoản Gmail: ");
        //fgets(account.username, sizeof(account.username), stdin);
        if (account.username[strlen(account.username) - 1] == '\n')
            account.username[strlen(account.username) - 1] = '\0';

        printf("Nhập mật khẩu Gmail: ");
        //fgets(account.password, sizeof(account.password), stdin);
        if (account.password[strlen(account.password) - 1] == '\n')
            account.password[strlen(account.password) - 1] = '\0';

        sslPOP = verifyAccount(account);
        if (sslPOP == NULL) {
            printLogo();
            printf("Sai tài khoản hoặc mật khẩu.\n");
        }
    }
    sslSMTP = loginSMTPServer(account);
    clearScreen();

}
void xemChiTietThu(int id){
    printLogo();
    EMAIL *email = getContent(sslPOP,id);
    printf("Tiêu đề: %s\nNgười gửi: %s\nNgười Nhận: %s\nNội dung:\n%s\n",email->subject,email->from,email->to,email->content);
    exit(0);
}

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
        xemChiTietThu(id);
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
    while (0 == 0)
        menu();
//    getTotalLetters(sslPOP);

    //showPageLetters(ssl, 1);
    //genHTML(getContent(ssl, 5)->html);
    return 0;
}