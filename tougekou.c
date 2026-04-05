#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_STUDENTS 200
#define NAME_LEN 40

typedef enum {
    CMD_EXIT = 0,
    CMD_SEARCH,
    CMD_ARRIVAL,
    CMD_DEPARTURE,
    CMD_LIST
} Command;

struct student {
    int id;
    char name[NAME_LEN];
    char school[NAME_LEN];
    int is_present;
    char last_event[20];
};

struct student_index {
    int id;
    int array_idx;
};

int load_data(FILE *fp, struct student *st_list, struct student_index *idx_list);
void quick_sort_idx(struct student_index *idx_list, int left, int right);
int find_by_id(int id, struct student_index *idx_list, int n);
void handle_attendance(struct student *st, int status);

int main(void) {
    struct student st_list[MAX_STUDENTS];
    struct student_index idx_list[MAX_STUDENTS];
    FILE *fp = fopen("seitodata.txt", "r");
    
    if (!fp) {
        perror("データファイルが開けません");
        return 1;
    }

    int count = load_data(fp, st_list, idx_list);
    fclose(fp);
    
    quick_sort_idx(idx_list, 0, count - 1);

    int cmd, target_id, found_idx;
    while (1) {
        printf("\n[ 塾生登下校管理システム ]\n");
        printf("1:検索 2:登校 3:下校 0:終了 > ");
        scanf("%d", &cmd);

        if (cmd == CMD_EXIT) break;

        switch (cmd) {
            case CMD_SEARCH:
                printf("生徒IDを入力: ");
                scanf("%d", &target_id);
                found_idx = find_by_id(target_id, idx_list, count);
                if (found_idx != -1) {
                    struct student s = st_list[found_idx];
                    printf("【照会】%s (%s) | 状態: %s | 最終打刻: %s\n", 
                           s.name, s.school, s.is_present ? "在室" : "帰宅済", s.last_event);
                } else {
                    printf("未登録のIDです。\n");
                }
                break;

            case CMD_ARRIVAL:
            case CMD_DEPARTURE:
                printf("生徒IDを入力: ");
                scanf("%d", &target_id);
                found_idx = find_by_id(target_id, idx_list, count);
                if (found_idx != -1) {
                    handle_attendance(&st_list[found_idx], cmd == CMD_ARRIVAL ? 1 : 0);
                } else {
                    printf("IDが見つかりません。\n");
                }
                break;

            default:
                printf("無効なコマンドです。\n");
        }
    }
    return 0;
}

void handle_attendance(struct student *st, int status) {
    if (st->is_present == status) {
        printf("エラー: 既に%s状態です。\n", status ? "登校" : "下校");
        return;
    }

    st->is_present = status;
    
    time_t t = time(NULL);
    struct tm *tm_now = localtime(&t);
    sprintf(st->last_event, "%02d:%02d:%02d", tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

    printf("確認: %sさんが%sしました。 [%s]\n", st->name, status ? "登校" : "下校", st->last_event);
}

int load_data(FILE *fp, struct student *st_list, struct student_index *idx_list) {
    int i = 0;
    while (fscanf(fp, "%d %s %s", &st_list[i].id, st_list[i].name, st_list[i].school) != EOF) {
        st_list[i].is_present = 0;
        strcpy(st_list[i].last_event, "--:--:--");
        idx_list[i].id = st_list[i].id;
        idx_list[i].array_idx = i;
        i++;
    }
    return i;
}
