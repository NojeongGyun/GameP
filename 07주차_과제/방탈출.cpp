#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <wchar.h>

void change_font() {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
    
    cfi.dwFontSize.X = 0;   
    cfi.dwFontSize.Y = 24;    
    cfi.FontWeight = FW_BOLD; 

    wcscpy(cfi.FaceName, L"GungsuhChe"); 
    
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

int get_cursor_y() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.dwCursorPosition.Y;
}

void play_item_sound() { Beep(784, 150); Beep(1046, 300); } 
void play_error_sound() { Beep(300, 300); }                 
void play_explosion_sound() { Beep(150, 500); Beep(100, 800); } 
void play_clear_sound() { Beep(523, 200); Beep(659, 200); Beep(784, 200); Beep(1046, 600); } 

void print_explosion() {
    system("cls");
    set_color(12);
    printf("\n==================================================\n");
    printf("                      쾅!!! ??                    \n");
    printf("                시간 초과! 폭탄 폭발!             \n");
    printf("==================================================\n");
    set_color(7);
    play_explosion_sound();
}

void print_story(const char* filename) {
    FILE *fp = fopen(filename, "r");
    char buffer[256];
    if (fp == NULL) {
        set_color(12);
        printf("\n[시스템] '%s' 파일을 찾을 수 없습니다.\n", filename);
        set_color(7);
        return;
    }
    set_color(11);
    printf("==================================================\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL) { 
        printf("%s", buffer); 
    }
    printf("\n==================================================\n");
    set_color(7);
    fclose(fp);
}

int get_user_input(int timer_started, time_t start_time, int *exploded, int timer_y) {
    char buf[10];
    int idx = 0;
    buf[0] = '\0';
    *exploded = 0;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int input_x = csbi.dwCursorPosition.X;
    int input_y = csbi.dwCursorPosition.Y;

    while (1) {
        if (timer_started) {
            int remain = 60 - (int)(time(NULL) - start_time);
            if (remain <= 0) {
                *exploded = 1; 
                print_explosion();
                return -1;
            }
            gotoxy(0, timer_y);
            set_color(12);
            printf("[ ?? 째깍째깍... 폭발까지 남은 시간 : %02d초! ]", remain);
            set_color(7);
            gotoxy(input_x + idx, input_y);
        }

        if (_kbhit()) {
            int c = _getch();
            if (c == '\r') {
                printf("\n");
                break;
            } else if (c == '\b') {
                if (idx > 0) {
                    idx--;
                    buf[idx] = '\0';
                    printf("\b \b");
                }
            } else if (c >= '0' && c <= '9') {
                if (idx < 9) {
                    buf[idx++] = (char)c;
                    buf[idx] = '\0';
                    printf("%c", c);
                }
            }
        }
        Sleep(50);
    }
    return atoi(buf);
}

int play_episode1(const char* filename) {
    int has_key_head = 0, has_key_body = 0, has_golden_key = 0, choice;
    int exploded; 

    print_story(filename);

    while (1) {
        printf("\n[ 에피소드 1 : 현재 행동을 선택하세요 ]\n");
        printf("1. 책상 조사하기\n");
        printf("2. 자물쇠 걸린 금고 조사하기\n");
        printf("3. 아이템 조합하기\n");
        printf("4. 굳게 닫힌 문 열기 (탈출 시도)\n");
        printf("선택: ");
        
        choice = get_user_input(0, 0, &exploded, 0); 
        system("cls");

        if (choice == 1) {
            if (has_key_head || has_golden_key) {
                printf("▶ 책상에는 더 이상 쓸만한 것이 없다.\n");
            } else {
                set_color(14);
                printf("▶ 책상 서랍을 열어보니 [부러진 열쇠 머리]가 있다! 획득했다.\n");
                set_color(7);
                play_item_sound(); has_key_head = 1;
            }
        } 
        else if (choice == 2) {
            if (has_key_body || has_golden_key) {
                printf("▶ 금고는 이미 텅 비어있다.\n");
            } else {
                int pw;
                printf("▶ 금고에 4자리 비밀번호가 필요하다. (힌트: 일이삼사)\n");
                printf("비밀번호 입력: ");
                pw = get_user_input(0, 0, &exploded, 0);
                
                if (pw == 1234) {
                    set_color(14);
                    printf("▶ 철컥! 금고가 열리며 [열쇠 손잡이]를 획득했다!\n");
                    set_color(7);
                    play_item_sound(); has_key_body = 1;
                } else {
                    set_color(12);
                    printf("▶ 삐빅! 비밀번호가 틀렸다.\n");
                    set_color(7);
                    play_error_sound(); 
                }
            }
        } 
        else if (choice == 3) {
            if (has_key_head == 1 && has_key_body == 1) {
                set_color(14);
                printf("▶ [부러진 열쇠 머리]와 [열쇠 손잡이]를 조합하여 [황금 열쇠]를 완성했다!\n");
                set_color(7);
                play_item_sound(); has_golden_key = 1; has_key_head = 0; has_key_body = 0;
            } else if (has_golden_key) {
                printf("▶ 이미 완벽한 열쇠를 가지고 있다.\n");
            } else {
                printf("▶ 조합할 수 있는 아이템이 부족하다.\n");
            }
        } 
        else if (choice == 4) {
            if (has_golden_key == 1) {
                set_color(10);
                printf("▶ [황금 열쇠]를 구멍에 넣고 돌리자 문이 열렸다!\n");
                printf("▶ 방탈출에 성공했습니다!\n");
                set_color(7);
                play_item_sound(); return 1; 
            } else {
                printf("▶ 문은 굳게 닫혀있다. 열쇠가 필요한 것 같다...\n");
            }
        } 
        else {
            printf("▶ 잘못된 선택입니다. 다시 선택해주세요.\n");
        }
    }
    return 0;
}

int play_episode2(const char* filename) {
    int has_flashlight = 0, has_battery = 0, has_working_flashlight = 0, choice;
    int timer_started = 0; 
    time_t start_time = 0;     

    print_story(filename);

    while (1) {
        printf("\n[ 에피소드 2 : 현재 행동을 선택하세요 ]\n");
        
        int timer_y = get_cursor_y(); 
        if (timer_started) {
            set_color(12);
            printf("[ ?? 째깍째깍... 폭발까지 남은 시간 : --초! ]\n");
            set_color(7);
        }

        printf("1. 낡은 책장 조사하기\n");
        printf("2. 기계 장치 조작하기\n");
        printf("3. 아이템 조합하기\n");
        printf("4. 어두운 통로로 진입하기 (탈출 시도)\n");
        printf("선택: ");

        int exploded;
        choice = get_user_input(timer_started, start_time, &exploded, timer_y);
        if (exploded) return -1;

        system("cls");

        if (choice == 1) {
            if (has_flashlight || has_working_flashlight) {
                printf("▶ 책장에는 먼지 쌓인 책들뿐이다.\n");
            } else {
                set_color(14);
                printf("▶ 두꺼운 책 뒤에서 [건전지가 없는 손전등]을 발견했다!\n");
                set_color(7); play_item_sound(); has_flashlight = 1;
            }
        } 
        else if (choice == 2) {
            if (timer_started == 0) {
                set_color(12);
                printf("▶ [경고] 기계 장치를 켜자 숨겨진 시한폭탄이 작동했습니다!\n");
                printf("▶ 남은 시간은 60초! 서둘러 방을 탈출하세요!\n\n");
                set_color(7);
                play_error_sound();
                start_time = time(NULL); 
                timer_started = 1;       
            }

            if (has_battery || has_working_flashlight) {
                printf("▶ 기계 장치 안쪽은 비어있다. 이미 필요한 걸 챙겼다.\n");
            } else {
                printf("▶ 기계 장치 화면에 수식이 적혀있다.\n");
                printf("▶ [ 50 X 40 + 26 = ? ]\n");
                
                int timer_y2 = get_cursor_y(); 
                set_color(12);
                printf("[ ?? 폭발까지 남은 시간 : --초! ]\n");
                set_color(7);
                
                printf("정답 입력: ");
                int answer = get_user_input(timer_started, start_time, &exploded, timer_y2);
                if (exploded) return -1; 
                
                if (answer == 2026) {
                    set_color(14);
                    printf("▶ 삐빅- 정답입니다. 서랍이 열리며 [AA 건전지]를 획득했다!\n");
                    set_color(7); play_item_sound(); has_battery = 1;
                } else {
                    set_color(12);
                    printf("▶ 부저음이 울린다. 오답인 것 같다.\n");
                    set_color(7); play_error_sound(); 
                }
            }
        } 
        else if (choice == 3) {
            if (has_flashlight == 1 && has_battery == 1) {
                set_color(14);
                printf("▶ [건전지가 없는 손전등]에 [AA 건전지]를 넣었다.\n");
                printf("▶ 불빛이 환하게 켜진다! [작동하는 손전등]을 얻었다!\n");
                set_color(7); play_item_sound(); 
                has_working_flashlight = 1; has_flashlight = 0; has_battery = 0;    
            } else if (has_working_flashlight) {
                printf("▶ 이미 손전등이 잘 작동하고 있다.\n");
            } else {
                printf("▶ 무언가 결합할 만한 아이템이 부족하다.\n");
            }
        } 
        else if (choice == 4) {
            if (has_working_flashlight == 1) {
                set_color(10);
                printf("▶ [작동하는 손전등]으로 어두운 통로를 비췄다.\n");
                printf("▶ 빛을 따라 걷다 보니 외부로 통하는 진짜 출구를 발견했다!\n");
                printf("▶ 에피소드 2 클리어! 완전히 탈출했습니다!\n");
                set_color(7); play_item_sound(); return 1; 
            } else {
                printf("▶ 통로가 너무 어두워서 한 발자국도 들어갈 수 없다. 빛이 필요하다...\n");
            }
        } 
        else {
            printf("▶ 잘못된 선택입니다. 다시 선택해주세요.\n");
        }
    }
    return 0;
}

int play_firewall_maze() {
    char maze[7][8] = {
        "#######",
        "#P  # #",
        "# ### #",
        "#     #",
        "### ###",
        "#    G#",
        "#######"
    };
    int px = 1, py = 1; 

    while (1) {
        system("cls");
        set_color(11);
        printf("\n==================================================\n");
        printf("           [ 서버 방화벽 우회 미니게임 ]          \n");
        printf(" 방향키(↑, ↓, ←, →)를 눌러 [P]를 [G]로 이동시키세요! \n");
        printf("==================================================\n\n");
        
        for (int i = 0; i < 7; i++) {
            printf("      "); 
            for (int j = 0; j < 7; j++) {
                if (maze[i][j] == 'P') { set_color(14); printf("P "); }
                else if (maze[i][j] == 'G') { set_color(10); printf("G "); }
                else if (maze[i][j] == '#') { set_color(8); printf("■"); }
                else { set_color(7); printf(". "); }
            }
            printf("\n");
        }
        set_color(7);

        if (px == 5 && py == 5) {
            set_color(10);
            printf("\n▶ 방화벽 우회 성공! 데이터 전송 완료!\n");
            set_color(7);
            play_item_sound();
            Sleep(1500); 
            return 1;
        }

        int key = _getch();
        if (key == 224) { 
            key = _getch(); 
            int nx = px, ny = py;
            
            if (key == 72) ny--;      
            else if (key == 80) ny++; 
            else if (key == 75) nx--; 
            else if (key == 77) nx++; 

            if (maze[ny][nx] != '#') {
                maze[py][px] = ' '; 
                px = nx; py = ny;   
                maze[py][px] = 'P'; 
            }
        }
    }
}

int play_episode3(const char* filename) {
    int system_unlocked = 0;   
    int firewall_bypassed = 0; 
    int has_master_key = 0;    
    int choice;
    int exploded; 

    print_story(filename);

    while (1) {
        printf("\n[ 에피소드 3 : 최종 행동을 선택하세요 ]\n");
        printf("1. 메인 단말기 접속 (날짜 암호 해제)\n");
        printf("2. 서버 방화벽 우회 (방향키 미니게임)\n");
        printf("3. 전자 금고 해킹 (숫자 야구 논리 퍼즐)\n");
        printf("4. 최종 보안문 열기 (탈출 시도)\n");
        printf("선택: ");
        
        choice = get_user_input(0, 0, &exploded, 0); 
        system("cls");

        if (choice == 1) {
            if (system_unlocked) {
                printf("▶ 시스템 권한이 이미 활성화되어 있습니다.\n");
            } else {
                int pw;
                set_color(11);
                printf("▶ [ 시스템 로그인 ]\n");
                printf("▶ 힌트: 시스템 재부팅 일자를 입력하시오. (YYYYMMDD)\n");
                printf("▶ 단말기 구석의 메모: '오늘 날짜(2026년 11월 4일)를 잊지 말 것!'\n");
                set_color(7);
                printf("비밀번호 8자리 입력: ");
                
                pw = get_user_input(0, 0, &exploded, 0);
                
                if (pw == 20261104) {
                    set_color(14);
                    printf("▶ '관리자 권한 승인.' 1차 보안 락이 해제되었습니다!\n");
                    set_color(7);
                    play_item_sound();
                    system_unlocked = 1;
                } else {
                    set_color(12);
                    printf("▶ 접근 거부. 형식이 틀렸거나 오늘 날짜가 아닙니다.\n");
                    set_color(7);
                    play_error_sound();
                }
            }
        } 
        else if (choice == 2) {
            if (firewall_bypassed) {
                printf("▶ 방화벽은 이미 해제되어 서버가 안정화되었습니다.\n");
            } else {
                if (play_firewall_maze() == 1) {
                    firewall_bypassed = 1; 
                    system("cls");
                    set_color(14);
                    printf("▶ 메인 서버의 방화벽이 완전히 무력화되었습니다!\n");
                    set_color(7);
                }
            }
        }
        else if (choice == 3) {
            if (has_master_key) {
                printf("▶ 금고는 열려있고, 이미 마스터 키를 챙겼다.\n");
            } else {
                set_color(12);
                printf("▶ [ 전자 금고 해킹 모드 ] - 기회는 단 7번!\n");
                printf("▶ 0~9 사이의 중복되지 않는 숫자 3자리를 맞춰야 합니다.\n");
                printf("▶ S(자리와 숫자 맞음), B(숫자만 맞음)\n\n");
                set_color(7);

                srand((unsigned int)time(NULL));
                int ans[3];
                ans[0] = rand() % 9 + 1; 
                do { ans[1] = rand() % 10; } while (ans[1] == ans[0]);
                do { ans[2] = rand() % 10; } while (ans[2] == ans[0] || ans[2] == ans[1]);

                printf("(치트키 - 정답: %d%d%d)\n", ans[0], ans[1], ans[2]); 

                int tries = 7;
                int success = 0;

                while (tries > 0) {
                    printf("[남은 기회: %d번] 3자리 숫자 입력: ", tries);
                    int guess = get_user_input(0, 0, &exploded, 0);
                    
                    int g[3] = { guess / 100, (guess / 10) % 10, guess % 10 };
                    int strike = 0, ball = 0;
                    for (int i = 0; i < 3; i++) {
                        for (int j = 0; j < 3; j++) {
                            if (g[i] == ans[j]) {
                                if (i == j) strike++; else ball++;
                            }
                        }
                    }

                    if (strike == 3) { success = 1; break; } 
                    else {
                        set_color(14);
                        printf("   => [ %d Strike, %d Ball ]\n", strike, ball);
                        set_color(7);
                        tries--;
                    }
                }

                if (success) {
                    set_color(10);
                    printf("\n▶ 찰칵! 방화벽이 뚫리며 금고가 열렸습니다!\n");
                    printf("▶ 안에서 [마스터 키]를 획득했습니다!\n");
                    set_color(7);
                    play_item_sound();
                    has_master_key = 1;
                } else {
                    set_color(12);
                    printf("\n▶ 해킹 실패... (정답: %d%d%d)\n", ans[0], ans[1], ans[2]);
                    set_color(7);
                    play_error_sound();
                }
            }
        } 
        else if (choice == 4) {
            if (system_unlocked && firewall_bypassed && has_master_key) {
                set_color(10);
                printf("▶ 마스터 키를 꽂자 거대한 최종 보안문이 열립니다!\n");
                printf("▶ 길고 길었던 방탈출에 마침내 성공했습니다!!\n");
                set_color(7);
                play_item_sound(); 
                return 1; 
            } else {
                printf("▶ 문이 꿈쩍도 하지 않는다. 아래 조건들을 확인하자.\n");
                printf("  [ %s ] 1차 시스템 암호 해제\n", system_unlocked ? "완료" : "미완료");
                printf("  [ %s ] 2차 방화벽 우회\n", firewall_bypassed ? "완료" : "미완료");
                printf("  [ %s ] 마스터 키 소지 여부\n", has_master_key ? "완료" : "미완료");
            }
        } 
        else {
            printf("▶ 잘못된 선택입니다. 다시 선택해주세요.\n");
        }
    }
    return 0;
}

int main(void) {
    change_font(); 
    
    system("cls");
    int exploded;
    
    if (play_episode1("에피소드1.txt") == 1) {
        set_color(10); 
        printf("\n===================================\n");
        printf("      에피소드 1 클리어 축하!      \n");
        printf("===================================\n");
        set_color(7); 
        
        printf("\n다음 방인 '에피소드2(어두운 통로)'로 이동하시겠습니까?\n");
        printf("1. 이동한다   2. 게임 종료\n선택: ");
        int next_ep2 = get_user_input(0, 0, &exploded, 0); 
        
        if (next_ep2 == 1) {
            system("cls");
            
            int ep2_result = play_episode2("에피소드2.txt");
            if (ep2_result == 1) {
                set_color(10); 
                printf("\n===================================\n");
                printf("      에피소드 2 클리어 축하!      \n");
                printf("===================================\n");
                set_color(7);

                printf("\n최종장인 '에피소드3(메인 통제실)'로 진입하시겠습니까?\n");
                printf("1. 진입한다   2. 게임 종료\n선택: ");
                int next_ep3 = get_user_input(0, 0, &exploded, 0);
                
                if (next_ep3 == 1) {
                    system("cls");
                    
                    int ep3_result = play_episode3("에피소드3.txt");
                    if (ep3_result == 1) {
                        set_color(10); 
                        printf("\n===================================\n");
                        printf("       ?? 모든 방을 탈출했습니다! ??     \n");
                        printf("       플레이해주셔서 감사합니다.      \n");
                        printf("===================================\n");
                        set_color(7);
                        play_clear_sound();
                    }
                } else {
                    printf("\n게임을 종료합니다. 수고하셨습니다!\n");
                }
            } 
        } else {
            printf("\n게임을 종료합니다. 수고하셨습니다!\n");
        }
    }
    return 0;
}
