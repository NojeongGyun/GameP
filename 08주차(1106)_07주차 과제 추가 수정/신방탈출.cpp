#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>
#include <time.h> 

// ?? 색상 매크로
#define C_DEFAULT 7
#define C_SUCCESS 10
#define C_INFO    11
#define C_ERROR   12
#define C_ITEM    14

void gotoxy(int x, int y);
void set_color(int color);
void set_console_font(void); 
void intro_game(void);
int select_episode(void);
void set_hint_mode(void);     
int prompt_difficulty(void); 
void show_rules(void);
int play_episode(int episode, int is_load); 
void display_scene_from_file(const char* filename);
void wait_for_esc(void);
int input_with_esc(char* buffer, int max_len); 
void clear_input_buffer(void);
void show_ending_stats(void); 
char get_rank(int seconds, int is_total); 
void animate_clear_screen(int episode, int final_time); 

void load_clear_data(void);
void save_clear_data(void);

// ?? 사운드 효과 함수들
void sound_navigate(void); 
void sound_item(void);     
void sound_success(void);  
void sound_error(void);    
void sound_action(void);   

// 전역 변수
int cheat_mode = 0;
int hint_mode = 1;       
int difficulty_mode = 3; 
int flow_hint_count = 0; 
char player_name[50];
int ep1_time = 0; 
int ep2_time = 0; 
int ep1_cleared = 0; 

int main(void)
{
    intro_game();

    while (1)
    {
        int choice = select_episode(); 

        if (choice == 0) { 
            if (!prompt_difficulty()) continue; 
            
            ep1_time = 0; ep2_time = 0;
            flow_hint_count = 0; 
            int is_cleared = play_episode(1, 0); 
            
            if (is_cleared == 1) {
                ep1_cleared = 1;
                save_clear_data();

                system("cls");
                set_color(C_SUCCESS);
                printf("==================================================\n");
                printf("   에피소드 1 클리어! 다음 스토리로 진입할까요?   \n");
                printf("==================================================\n\n");
                set_color(C_DEFAULT);
                printf(" >> 에피소드 2: [닫힌 상담실의 비밀]을 바로 시작하시겠습니까?\n");
                printf("    ( Y : 바로 시작 / N : 메인 메뉴로 돌아가기 )\n\n▶ ");
                
                while (1) {
                    int k = getch();
                    if (k == 'y' || k == 'Y') {
                        if (!prompt_difficulty()) break; 
                        flow_hint_count = 0; 
                        int clear2 = play_episode(2, 0); 
                        if (clear2 == 1) show_ending_stats(); 
                        break;
                    } else if (k == 'n' || k == 'N') {
                        break; 
                    }
                }
            }
        } 
        else if (choice == 1) { 
            if (!prompt_difficulty()) continue; 
            ep2_time = 0;
            flow_hint_count = 0;
            int clear2 = play_episode(2, 0);
            if (clear2 == 1) show_ending_stats(); 
        } 
        else if (choice == 2) { 
            FILE* fp = fopen("save.dat", "r");
            if (fp == NULL) {
                system("cls");
                set_color(C_ERROR);
                printf("==================================================\n");
                printf("             저장된 데이터가 없습니다!            \n");
                printf("==================================================\n");
                set_color(C_DEFAULT);
                sound_error();
                wait_for_esc();
            } else {
                int saved_episode;
                fscanf(fp, "%d", &saved_episode); 
                fclose(fp);
                
                int clear_loaded = play_episode(saved_episode, 1); 
                if (clear_loaded == 1) {
                    if (saved_episode == 1) {
                        ep1_cleared = 1;      
                        save_clear_data();

                        system("cls");
                        set_color(C_SUCCESS);
                        printf("==================================================\n");
                        printf("   에피소드 1 클리어! 다음 스토리로 진입할까요?   \n");
                        printf("==================================================\n\n");
                        set_color(C_DEFAULT);
                        printf(" >> 에피소드 2 바로 시작? (Y/N)\n▶ ");
                        while (1) {
                            int k = getch();
                            if (k == 'y' || k == 'Y') {
                                if (!prompt_difficulty()) break; 
                                flow_hint_count = 0;
                                int clear2 = play_episode(2, 0);
                                if (clear2 == 1) show_ending_stats();
                                break;
                            } else if (k == 'n' || k == 'N') {
                                break;
                            }
                        }
                    } else if (saved_episode == 2) {
                        show_ending_stats();
                    }
                }
            }
        }
        else if (choice == 3) { 
            show_rules();
        } 
        else if (choice == 4) { 
            set_hint_mode(); 
        } 
        else if (choice == 5) { 
            printf("\n게임을 종료합니다. 감사합니다!\n");
            break;
        }
    }
    return 0;
}

// ??? 폰트 및 크기 설정 함수 (바탕체 & 크기 24 적용)
void set_console_font(void) {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    cfi.nFont = 0;
    
    // 폭을 0으로 두어 세로(Y) 크기에 맞춰 윈도우가 자동 계산하도록 위임
    cfi.dwFontSize.X = 0; 
    cfi.dwFontSize.Y = 24; // 20이 작을 수 있어 24로 큼직하게 변경
    
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    // 윈도우 콘솔이 그나마 잘 받아주는 "바탕체"의 유니코드 주입
    cfi.FaceName[0] = (WCHAR)0xBC14; // '바'
    cfi.FaceName[1] = (WCHAR)0xD0D5; // '탕'
    cfi.FaceName[2] = (WCHAR)0xCC68; // '체'
    cfi.FaceName[3] = (WCHAR)0x0000; // 문자열 끝

    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

// ?? 색상 변경 함수
void set_color(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ?? 상황별 사운드 함수
void sound_navigate(void) { Beep(500, 40); }
void sound_item(void) { Beep(523, 100); Beep(659, 100); Beep(784, 150); }
void sound_success(void) { Beep(659, 100); Beep(784, 100); Beep(1046, 250); }
void sound_error(void) { Beep(300, 150); Beep(200, 200); }
void sound_action(void) { Beep(800, 60); }

void load_clear_data(void) {
    FILE* fp = fopen("clear.dat", "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &ep1_cleared);
        fclose(fp);
    }
}

void save_clear_data(void) {
    FILE* fp = fopen("clear.dat", "w");
    if (fp != NULL) {
        fprintf(fp, "%d", ep1_cleared);
        fclose(fp);
    }
}

void gotoxy(int x, int y) {
    COORD pos = { (short)x, (short)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// ?? 에피소드 시작 전 난이도 묻기
int prompt_difficulty(void) {
    int choice = 0; 
    int key;
    
    while (1) {
        system("cls");
        printf("==================================================\n");
        printf("              이번 게임의 난이도 설정             \n");
        printf("==================================================\n");
        set_color(C_INFO);
        printf("              ( [ESC] 키를 누르면 취소 )            \n\n");
        set_color(C_DEFAULT);
        
        char opt0[150];
        if (hint_mode == 1) strcpy(opt0, "기존 설정 유지 (흐름 힌트 무제한 / [단서] 기호 보임 [ON])");
        else strcpy(opt0, "기존 설정 유지 (흐름 힌트 무제한 / 단서 기호 숨김 [OFF])");

        const char* options[] = { 
            opt0,
            "EASY   (흐름 힌트 무제한 / [단서] 기호 보임)", 
            "NORMAL (흐름 힌트 총 3회 제한 / 단서 기호 숨김)",
            "HARD   (흐름 힌트 불가 / 단서 기호 숨김)" 
        };
        
        for (int i = 0; i < 4; i++) {
            if (i == choice) {
                set_color(C_ITEM);
                printf("    ▶ %s ◀\n", options[i]);
                set_color(C_DEFAULT);
            }
            else printf("       %s\n", options[i]);
        }
        
        key = getch();
        if (key == 27) { 
            return 0; 
        }
        else if (key == 224 || key == 0) {
            key = getch();
            if (key == 72) { choice = (choice - 1 + 4) % 4; sound_navigate(); }
            else if (key == 80) { choice = (choice + 1) % 4; sound_navigate(); }
        } else if (key == 13) {
            sound_action();
            if (choice == 0) difficulty_mode = 3;      
            else if (choice == 1) difficulty_mode = 2; 
            else if (choice == 2) difficulty_mode = 1; 
            else if (choice == 3) difficulty_mode = 0; 
            return 1; 
        }
    }
}

void set_hint_mode(void) {
    int choice = (hint_mode == 1) ? 0 : 1; 
    int key;
    while (1) {
        system("cls");
        printf("==================================================\n");
        printf("           메인 메뉴 힌트 모드 설정               \n");
        printf("==================================================\n\n");
        const char* options[] = { 
            "켜기 (ON) - 사물 조사 시 [단서] 괄호 표시", 
            "끄기 (OFF) - [단서] 괄호 숨김 (직접 추리)" 
        };
        for (int i = 0; i < 2; i++) {
            if (i == choice) {
                set_color(C_ITEM);
                printf("    ▶ %s ◀\n", options[i]);
                set_color(C_DEFAULT);
            }
            else printf("       %s\n", options[i]);
        }
        key = getch();
        if (key == 224 || key == 0) {
            key = getch();
            if (key == 72) { choice = (choice - 1 + 2) % 2; sound_navigate(); }
            else if (key == 80) { choice = (choice + 1) % 2; sound_navigate(); }
        } else if (key == 13) {
            sound_action();
            if (choice == 0) hint_mode = 1; else hint_mode = 0;
            break; 
        }
    }
}

void animate_clear_screen(int episode, int final_time) {
    system("cls");
    sound_success(); 
    
    for(int i = 0; i < 15; i++) {
        int y_offset = i; 
        if (y_offset > 8) y_offset = 8; 
        
        int color = (i % 3 == 0) ? 10 : ((i % 3 == 1) ? 14 : 11);
        set_color(color);
        
        gotoxy(0, y_offset);
        printf("==================================================\n");
        if (episode == 1) printf(" [ 에피소드 1 클리어 ] 마침내 모든 층을 빠져나왔습니다! \n");
        else printf(" [ 에피소드 2 클리어 ] 닫힌 상담실 탈출 성공! \n");
        printf("==================================================\n");
        
        Sleep(120); 
        
        if (i < 14) {
            gotoxy(0, y_offset);
            printf("                                                  \n");
            printf("                                                          \n");
            printf("                                                  \n");
        }
    }
    set_color(C_DEFAULT);
    
    gotoxy(0, 12);
    printf(" >> 챕터 소요 시간 : %d분 %d초\n", final_time / 60, final_time % 60);
    printf(" >> 챕터 획득 랭크 : [ %c ] 등급\n\n", get_rank(final_time, 0));
    wait_for_esc();
}

char get_rank(int seconds, int is_total) {
    int multiplier = is_total ? 2 : 1; 
    if (seconds <= 120 * multiplier) return 'S';      
    else if (seconds <= 240 * multiplier) return 'A'; 
    else if (seconds <= 420 * multiplier) return 'B'; 
    else if (seconds <= 720 * multiplier) return 'C'; 
    else if (seconds <= 1200 * multiplier) return 'D';
    else return 'F';                                  
}

void show_ending_stats(void) {
    system("cls");
    set_color(C_ITEM);
    printf("==================================================\n");
    printf("              [ 게 임  최 종  결 과 ]             \n");
    printf("==================================================\n\n");
    set_color(C_DEFAULT);
    
    if (ep1_time > 0) 
        printf(" - 에피소드 1 소요시간 : %d분 %d초 (랭크: %c)\n", ep1_time/60, ep1_time%60, get_rank(ep1_time, 0));
    if (ep2_time > 0)
        printf(" - 에피소드 2 소요시간 : %d분 %d초 (랭크: %c)\n", ep2_time/60, ep2_time%60, get_rank(ep2_time, 0));
    
    int t_time = ep1_time + ep2_time;
    printf("--------------------------------------------------\n");
    set_color(C_SUCCESS);
    printf(" ▶ 총 누적 소요시간  : %d분 %d초\n", t_time/60, t_time%60);
    printf(" ▶ 최종 종합 랭크    : [ %c ]\n", get_rank(t_time, 1));
    set_color(C_DEFAULT);
    
    printf("\n [ 난이도 보너스 정보 ]\n");
    if (difficulty_mode == 0) printf("  HARD 모드로 클리어하셨습니다! 대단한 추리력입니다!\n");
    else if (difficulty_mode == 1) printf("  NORMAL 모드로 클리어하셨습니다. 완벽한 밸런스!\n");
    else if (difficulty_mode == 2) printf("  EASY 모드로 클리어하셨습니다. 다음엔 NORMAL에 도전해 보세요!\n");
    else {
        if (hint_mode == 1) printf("  기본 모드(단서 보임)로 클리어하셨습니다. 다음엔 NORMAL에 도전해 보세요!\n");
        else printf("  기본 모드(단서 숨김)로 클리어하셨습니다. 훌륭한 추리력입니다!\n");
    }
    
    printf("==================================================\n\n");
    wait_for_esc();
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void wait_for_esc(void) {
    set_color(8); 
    printf("\n[ ESC 키를 누르면 돌아갑니다... ]\n");
    set_color(C_DEFAULT);
    while (1) { if (getch() == 27) break; }
}

int input_with_esc(char* buffer, int max_len) {
    int idx = 0;
    while (1) {
        int ch = getch();
        if (ch == 27) return 0; 
        else if (ch == '\r' || ch == '\n') { buffer[idx] = '\0'; printf("\n"); return 1; }
        else if (ch == '\b') { if (idx > 0) { idx--; printf("\b \b"); } }
        else if (ch == 224 || ch == 0) getch();
        else if (idx < max_len - 1) { buffer[idx++] = (char)ch; putchar(ch); }
    }
}

void intro_game(void) {
    // ??? 폰트 먼저 적용
    set_console_font(); 

    load_clear_data(); 
    
    system("title 어드벤처 방탈출 v26.3 (바탕체 및 폰트 크기 24 적용)");
    
    // 윈도우 크기와 버퍼 설정
    system("mode con cols=65 lines=25"); 
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 780, 600, TRUE); 
    
    system("cls");
    set_color(C_ITEM);
    printf("==================================================\n");
    printf("           어드벤처 텍스트 방탈출 게임            \n");
    printf("==================================================\n\n");
    set_color(C_DEFAULT);
    printf("당신의 이름을 입력하세요: ");
    scanf("%49s", player_name);
    clear_input_buffer(); 
    
    if (strcmp(player_name, "노정균") == 0) cheat_mode = 1; 
    else cheat_mode = 0;
}

int select_episode(void) {
    int choice = 0; 
    int key;
    char hint_status[50];
    char ep2_menu[100];
    
    while (1) {
        system("cls");
        printf("==================================================\n");
        printf("                 메인 메뉴 선택                   \n");
        printf("==================================================\n\n");
        set_color(8);
        printf("       (방향키 상/하 [↑][↓] 이동 후 [Enter])\n\n");
        set_color(C_DEFAULT);

        sprintf(hint_status, "힌트 모드 설정 (현재: %s)", hint_mode == 1 ? "ON" : "OFF");

        if (ep1_cleared || cheat_mode) {
            strcpy(ep2_menu, "에피소드 2 : [닫힌 상담실의 비밀] 시작");
        } else {
            strcpy(ep2_menu, "에피소드 2 : [잠김] (에피소드 1 클리어 필요)");
        }

        const char* options[] = {
            "에피소드 1 : [비밀 사무실의 밤] 시작",
            ep2_menu,
            "불러오기 (저장된 게임 이어하기)",
            "게임 설명 및 규칙 읽기",
            hint_status,
            "게임 종료"
        };

        for (int i = 0; i < 6; i++) {
            if (i == choice) {
                set_color(C_ITEM);
                printf("    ▶ %s ◀\n", options[i]);
                set_color(C_DEFAULT);
            }
            else {
                if (i == 1 && !ep1_cleared && !cheat_mode) set_color(8); 
                printf("       %s\n", options[i]);
                set_color(C_DEFAULT);
            }
        }

        key = getch();
        if (key == 224 || key == 0) {
            key = getch();
            if (key == 72) { choice--; if (choice < 0) choice = 5; sound_navigate(); } 
            else if (key == 80) { choice++; if (choice > 5) choice = 0; sound_navigate(); }
        } else if (key == 13) {
            if (choice == 1 && !ep1_cleared && !cheat_mode) {
                set_color(C_ERROR);
                printf("\n\n   [시스템] 에피소드 1을 먼저 클리어해야 입장할 수 있습니다!\n");
                set_color(C_DEFAULT);
                sound_error();
                Sleep(1500); 
                continue;
            }
            sound_action();
            return choice;
        }
    }
}

void show_rules(void) {
    system("cls");
    printf("==================================================\n");
    printf("                  [ 게임 설명 ]                   \n");
    printf("==================================================\n\n");
    printf("1. 방 안의 묘사를 읽고 의심 가는 '사물'을 조사하세요.\n");
    printf("2. [ 난이도 시스템 ] (에피소드 시작 시 선택)\n");
    printf(" - 기존 설정 : 흐름 힌트 무제한 + 괄호 표시는 메인메뉴 ON/OFF 연동\n");
    printf(" - EASY : 단서에 [ ] 표시. 흐름 힌트 무제한!\n");
    printf(" - NORMAL : 괄호 숨김. 흐름 힌트 3회 제한.\n");
    printf(" - HARD : 괄호 숨김 & 흐름 힌트 없음! 순수 실력으로 탈출.\n\n");
    printf("3. [특수 명령어]\n");
    printf(" - 인벤토리 / 조합 / 분해 / 힌트\n");
    printf(" - 0 : 그냥 나가기 / 9 : 현재 진행 상황 저장하고 나가기\n\n");
    printf("4. 단서를 본 뒤엔 [ESC] 키로 돌아오세요.\n\n");
    wait_for_esc();
}

// ==========================================
// 메인 게임 플레이 로직
// ==========================================
int play_episode(int episode, int is_load)
{
    char user_answer[50];
    char filename[100]; 
    char obj_file[150]; 

    int current_room = 1; 
    int is_escaped = 0;
    int is_door_unlocked = 0; 

    int has_id_card = 0, has_battery = 0, has_broken_flash = 0, has_flash = 0, has_key1 = 0;
    int has_remote = 0, has_working_remote = 0; 
    int has_golden_key = 0, has_usb = 0, has_masterkey = 0;  
    
    time_t start_time = time(NULL);
    int acc_time = 0; 

    if (is_load == 1) {
        FILE* fp = fopen("save.dat", "r");
        if (fp != NULL) {
            int dummy_ep; 
            fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                &dummy_ep, &current_room, &is_door_unlocked, 
                &has_id_card, &has_battery, &has_broken_flash, &has_flash, &has_key1,
                &has_remote, &has_working_remote, &has_golden_key, &has_usb, &has_masterkey,
                &acc_time, &ep1_time, &ep2_time, &difficulty_mode, &flow_hint_count, &hint_mode);
            fclose(fp);
            
            system("cls");
            set_color(C_SUCCESS);
            printf("==================================================\n");
            printf("   저장된 데이터를 성공적으로 불러왔습니다!   \n");
            printf("==================================================\n");
            set_color(C_DEFAULT);
            sound_success();
            wait_for_esc();
        }
    } 
    else if (cheat_mode) {
        if (episode == 1) {
            has_id_card = 1; has_battery = 1; has_broken_flash = 1; has_flash = 1;
            has_key1 = 1; has_remote = 1; has_working_remote = 1;
            has_golden_key = 1; has_usb = 1;
        } else if (episode == 2) {
            has_masterkey = 1;
        }
    }

    char room_names[6][20]; 
    if (episode == 1) {
        strcpy(room_names[1], "비밀사무실1"); strcpy(room_names[2], "복도"); 
        strcpy(room_names[3], "탕비실"); strcpy(room_names[4], "비밀사무실2"); 
        strcpy(room_names[5], "비밀사무실3"); 
    } else {
        strcpy(room_names[1], "상담실"); strcpy(room_names[2], "원장실"); 
        strcpy(room_names[3], ""); strcpy(room_names[4], ""); strcpy(room_names[5], "");
    }

    while (!is_escaped)
    {
        system("cls");
        printf("==================================================\n");
        printf(" 현재 위치 : [");
        set_color(C_INFO);
        printf("%s", room_names[current_room]);
        set_color(C_DEFAULT);
        printf("]\n==================================================\n\n");
        
        sprintf(filename, "%s.txt", room_names[current_room]);
        display_scene_from_file(filename);

        printf("\n\n--------------------------------------------------\n");
        set_color(8); 
        printf("[ 명령어: 인벤토리, 조합, 분해, 힌트, 사물/장소 이름 ]\n"); 
        set_color(C_DEFAULT);
        
        if (cheat_mode) {
            set_color(C_SUCCESS);
            printf("[VIP 치트] 시작부터 모든 아이템을 소지하고 있습니다!\n");
            if (episode == 1) printf("[정답] 단말기: 20261106 / 금고: 7777 / 엘베: 1024\n");
            else printf("[정답] 도어락: 4321 / 상자: 일기장\n");
            set_color(C_DEFAULT);
        }
        
        printf("▶ 행동 입력 (0: 나가기, 9: 저장 후 나가기): ");
        scanf("%49s", user_answer);
        clear_input_buffer(); 
        
        if (strcmp(user_answer, "0") == 0) {
            return 0; 
        }

        if (strcmp(user_answer, "9") == 0 || strcmp(user_answer, "저장") == 0) {
            FILE* fp = fopen("save.dat", "w");
            if (fp != NULL) {
                int current_acc = acc_time + (int)difftime(time(NULL), start_time);
                fprintf(fp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
                    episode, current_room, is_door_unlocked, 
                    has_id_card, has_battery, has_broken_flash, has_flash, has_key1,
                    has_remote, has_working_remote, has_golden_key, has_usb, has_masterkey,
                    current_acc, ep1_time, ep2_time, difficulty_mode, flow_hint_count, hint_mode);
                fclose(fp);
                
                system("cls");
                set_color(C_SUCCESS);
                printf("==================================================\n");
                printf("     진행 상황이 안전하게 저장되었습니다!     \n");
                printf("==================================================\n\n");
                set_color(C_DEFAULT);
                printf("메인 메뉴로 돌아갑니다.\n");
                sound_success();
                wait_for_esc();
            }
            return 0; 
        }

        if (strcmp(user_answer, "인벤토리") == 0) {
            system("cls");
            printf("==================================================\n");
            printf("                  [ 인벤토리 ]                    \n");
            printf("==================================================\n\n");
            set_color(C_ITEM);
            int empty = 1;
            if(has_id_card) { printf("- 사원증\n"); empty = 0; }
            if(has_battery) { printf("- 건전지 (AAA 사이즈)\n"); empty = 0; }
            if(has_broken_flash) { printf("- 고장난손전등\n"); empty = 0; }
            if(has_flash) { printf("- 손전등 (빛이 밝다)\n"); empty = 0; }
            if(has_key1) { printf("- 비상문 열쇠\n"); empty = 0; }
            if(has_remote) { printf("- 리모컨 (건전지가 없다)\n"); empty = 0; }
            if(has_working_remote) { printf("- 작동하는리모컨\n"); empty = 0; }
            if(has_golden_key) { printf("- 황금열쇠 (눈부시게 빛난다)\n"); empty = 0; }
            if(has_usb) { printf("- 보안USB (빨간불이 들어온다)\n"); empty = 0; }
            if(has_masterkey) { printf("- 마스터키\n"); empty = 0; }
            if(empty) printf("  (가진 아이템이 없습니다)\n");
            set_color(C_DEFAULT);
            sound_action();
            wait_for_esc();
            continue;
        }
        else if (strcmp(user_answer, "조합") == 0) {
            system("cls");
            printf("==================================================\n");
            printf("                [ 아이템 조합 ]                  \n");
            printf("==================================================\n\n");
            
            printf(" [★] 현재 가진 아이템 목록:\n");
            set_color(C_ITEM);
            int empty = 1;
            if(has_id_card) { printf("  - 사원증\n"); empty = 0; }
            if(has_battery) { printf("  - 건전지\n"); empty = 0; }
            if(has_broken_flash) { printf("  - 고장난손전등\n"); empty = 0; }
            if(has_flash) { printf("  - 손전등\n"); empty = 0; }
            if(has_key1) { printf("  - 비상문 열쇠\n"); empty = 0; }
            if(has_remote) { printf("  - 리모컨\n"); empty = 0; }
            if(has_working_remote) { printf("  - 작동하는리모컨\n"); empty = 0; }
            if(has_golden_key) { printf("  - 황금열쇠\n"); empty = 0; }
            if(has_usb) { printf("  - 보안USB\n"); empty = 0; }
            if(has_masterkey) { printf("  - 마스터키\n"); empty = 0; }
            if(empty) printf("  (가진 아이템이 없습니다)\n");
            set_color(C_DEFAULT);
            
            printf("\n--------------------------------------------------\n");
            printf(" [ESC] 취소하고 돌아가기 / [Enter] 조합 시작\n");

            int key;
            while(1) { key = getch(); if (key == 27 || key == 13) break; }
            if (key == 27) continue; 

            char item1[50], item2[50];
            printf("\n첫 번째 템 (취소: 0): "); scanf("%49s", item1); clear_input_buffer(); 
            if (strcmp(item1, "0") == 0) continue;
            printf("두 번째 템 (취소: 0): "); scanf("%49s", item2); clear_input_buffer(); 
            if (strcmp(item2, "0") == 0) continue;

            if ((strcmp(item1, "건전지") == 0 && strcmp(item2, "고장난손전등") == 0) ||
                (strcmp(item1, "고장난손전등") == 0 && strcmp(item2, "건전지") == 0)) 
            {
                if (has_battery && has_broken_flash) {
                    set_color(C_ITEM);
                    printf("\n>> 건전지와 고장난손전등을 조합!\n>> [손전등]을 만들었습니다!\n");
                    set_color(C_DEFAULT);
                    sound_item();
                    has_battery = 0; has_broken_flash = 0; has_flash = 1;
                } else { set_color(C_ERROR); printf("\n>> 해당 아이템이 부족합니다.\n"); set_color(C_DEFAULT); sound_error(); }
            } 
            else if ((strcmp(item1, "건전지") == 0 && strcmp(item2, "리모컨") == 0) ||
                     (strcmp(item1, "리모컨") == 0 && strcmp(item2, "건전지") == 0)) 
            {
                if (has_battery && has_remote) {
                    set_color(C_ITEM);
                    printf("\n>> 리모컨에 AAA 건전지를 끼워 넣었습니다!\n>> [작동하는리모컨]을 완성했습니다!\n");
                    set_color(C_DEFAULT);
                    sound_item();
                    has_battery = 0; has_remote = 0; has_working_remote = 1;
                } else { set_color(C_ERROR); printf("\n>> 해당 아이템이 부족합니다.\n"); set_color(C_DEFAULT); sound_error(); }
            }
            else {
                set_color(C_ERROR); printf("\n>> 서로 조합할 수 없거나 없는 템입니다.\n"); set_color(C_DEFAULT); sound_error();
            }
            wait_for_esc();
            continue;
        }
        else if (strcmp(user_answer, "분해") == 0) {
            system("cls");
            printf("==================================================\n");
            printf("                [ 아이템 분해 ]                  \n");
            printf("==================================================\n\n");
            
            printf(" [★] 현재 가진 아이템 목록:\n");
            set_color(C_ITEM);
            int empty = 1;
            if(has_id_card) { printf("  - 사원증\n"); empty = 0; }
            if(has_battery) { printf("  - 건전지\n"); empty = 0; }
            if(has_broken_flash) { printf("  - 고장난손전등\n"); empty = 0; }
            if(has_flash) { printf("  - 손전등\n"); empty = 0; }
            if(has_key1) { printf("  - 비상문 열쇠\n"); empty = 0; }
            if(has_remote) { printf("  - 리모컨\n"); empty = 0; }
            if(has_working_remote) { printf("  - 작동하는리모컨\n"); empty = 0; }
            if(has_golden_key) { printf("  - 황금열쇠\n"); empty = 0; }
            if(has_usb) { printf("  - 보안USB\n"); empty = 0; }
            if(has_masterkey) { printf("  - 마스터키\n"); empty = 0; }
            if(empty) printf("  (가진 아이템이 없습니다)\n");
            set_color(C_DEFAULT);
            
            printf("\n--------------------------------------------------\n");
            printf(" [ESC] 취소하고 돌아가기 / [Enter] 분해 시작\n");

            int key;
            while(1) { key = getch(); if (key == 27 || key == 13) break; }
            if (key == 27) continue; 

            char d_item[50];
            printf("\n분해할 아이템 이름 입력 (취소: 0)\n▶ "); 
            scanf("%49s", d_item); clear_input_buffer();
            if (strcmp(d_item, "0") == 0) continue;

            if (strcmp(d_item, "손전등") == 0) {
                if (has_flash) {
                    set_color(C_ITEM);
                    printf("\n>> 손전등을 분해했습니다!\n");
                    printf(">> [건전지]와 [고장난손전등]을 다시 얻었습니다.\n");
                    set_color(C_DEFAULT);
                    sound_item();
                    has_flash = 0; has_battery = 1; has_broken_flash = 1;
                } else { set_color(C_ERROR); printf("\n>> 손전등을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error(); }
            } 
            else if (strcmp(d_item, "작동하는리모컨") == 0) {
                if (has_working_remote) {
                    set_color(C_ITEM);
                    printf("\n>> 리모컨에서 배터리를 뺐습니다!\n");
                    printf(">> [건전지]와 [리모컨]으로 분리되었습니다.\n");
                    set_color(C_DEFAULT);
                    sound_item();
                    has_working_remote = 0; has_battery = 1; has_remote = 1;
                } else { set_color(C_ERROR); printf("\n>> 해당 아이템을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error(); }
            }
            else {
                set_color(C_ERROR); printf("\n>> 분해할 수 없는 물건입니다.\n"); set_color(C_DEFAULT); sound_error();
            }
            wait_for_esc();
            continue;
        }
        else if (strcmp(user_answer, "힌트") == 0) {
            system("cls");
            printf("==================================================\n");
            printf("                  [ 흐름 힌트 ]                   \n");
            printf("==================================================\n\n");
            
            if (difficulty_mode == 0) {
                set_color(C_ERROR);
                printf(">> [!] HARD 모드에서는 흐름 힌트를 사용할 수 없습니다.\n");
                set_color(C_DEFAULT);
                sound_error();
            } 
            else if (difficulty_mode == 1 && flow_hint_count >= 3) {
                set_color(C_ERROR);
                printf(">> [!] NORMAL 모드의 흐름 힌트를 모두 소진했습니다. (3/3)\n");
                printf(">> 이제 당신의 추리력만 믿어야 합니다!\n");
                set_color(C_DEFAULT);
                sound_error();
            } 
            else {
                sound_action();
                if (difficulty_mode == 1) {
                    flow_hint_count++;
                    set_color(C_INFO);
                    printf(">> [※] 남은 흐름 힌트 횟수 : %d회 (총 3회)\n\n", 3 - flow_hint_count);
                    set_color(C_DEFAULT);
                }

                if (episode == 1) {
                    if (current_room == 1) { 
                        if (!has_id_card) printf(">> 외투나 책상 등 주변을 샅샅이 조사하여 단말기를 켤 물건을 찾으세요.\n");
                        else if (!is_door_unlocked) printf(">> [사원증]을 얻었으니 [단말기]를 조작해보세요. 비밀번호는 달력과 메모장을 참고!\n");
                        else printf(">> 문이 열렸습니다. [도어락]을 통해 [복도]로 이동하세요.\n");
                    }
                    else if (current_room == 2 || current_room == 3) { 
                        if (!has_broken_flash && !has_flash) printf(">> 어두운 곳을 밝힐 물건이 필요합니다. [복도]의 서랍을 조사해보세요.\n");
                        else if (has_battery && has_broken_flash) printf(">> 고장난 손전등과 건전지가 있습니다. [조합] 명령어를 써서 고쳐보세요.\n");
                        else if (has_flash && !has_key1) printf(">> [손전등]이 생겼으니 [탕비실]로 이동해 [환풍구] 안을 확인해보세요!\n");
                        else if (has_key1) printf(">> [비상문 열쇠]를 얻었습니다. [복도]에 있는 [비상문]을 열어보세요.\n");
                    }
                    else if (current_room == 4) { 
                        if (!has_remote && !has_working_remote) printf(">> 주변에 있는 낡은 [캐비닛]을 먼저 조사해보세요.\n");
                        else if (has_remote && !has_battery) printf(">> 리모컨에 넣을 건전지가 필요합니다. 쓸모를 다한 [손전등]을 [분해]해보는 건 어떨까요?\n");
                        else if (has_remote && has_battery) printf(">> 건전지를 얻었습니다! [조합] 명령어로 리모컨과 합쳐보세요.\n");
                        else if (has_working_remote && !has_golden_key) printf(">> 완성된 [작동하는리모컨]을 사용해 스크린을 켜고, [스크린]을 조사해 [금고]를 여세요.\n");
                        else if (has_golden_key) printf(">> [황금열쇠]를 얻었습니다! 이제 굳게 닫힌 [철문]을 여세요.\n");
                    }
                    else if (current_room == 5) { 
                        if (!has_usb) printf(">> 마지막 방입니다. [서류가방]을 조사해 보안 키를 찾으세요.\n");
                        else printf(">> 얻은 [보안USB]를 [컴퓨터]에 꽂고, 힌트를 얻어 [엘리베이터] 단말기에 암호를 입력하세요!\n");
                    }
                } 
                else if (episode == 2) {
                    if (current_room == 1) { 
                        if (!is_door_unlocked) printf(">> [도어락] 비밀번호를 풀어야 다음 방으로 갈 수 있습니다.\n");
                        else printf(">> 문이 열렸습니다. [원장실]로 이동하세요.\n");
                    } 
                    else if (current_room == 2) { 
                        if (!has_masterkey) printf(">> [상자]의 비밀번호를 풀어야 합니다. 주변을 잘 살펴보세요.\n");
                        else printf(">> [마스터키]를 얻었습니다! [비상구]를 통해 완전히 탈출하세요!\n");
                    }
                }
            }
            wait_for_esc();
            continue;
        }

        int is_event = 0;

        if (episode == 1) {
            if (current_room == 1) { 
                if (strcmp(user_answer, "외투") == 0) {
                    system("cls");
                    if (!has_id_card) { 
                        set_color(C_ITEM); printf("\n>> 안주머니에서 [사원증] 획득!\n"); set_color(C_DEFAULT);
                        sound_item(); has_id_card = 1; 
                    } else {
                        set_color(C_INFO); printf("\n>> 외투 주머니는 텅 비어있다.\n"); set_color(C_DEFAULT); sound_action();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "책상") == 0) {
                    system("cls");
                    if (!has_battery) { 
                        set_color(C_ITEM); printf("\n>> 서랍에서 [건전지] 획득!\n"); set_color(C_DEFAULT);
                        sound_item(); has_battery = 1; 
                    } else {
                        set_color(C_INFO); printf("\n>> 더 이상 쓸만한 건 없다.\n"); set_color(C_DEFAULT); sound_action();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "메모장") == 0) {
                    system("cls");
                    set_color(C_INFO);
                    printf("\n[ 메모장 조사 ]\n");
                    printf("--------------------------------------------------\n");
                    printf("\"출입구 단말기 비밀번호는 항상 '오늘 날짜' 8자리로 유지할 것!\"\n");
                    printf("--------------------------------------------------\n");
                    set_color(C_DEFAULT);
                    sound_action();
                    is_event = 1;
                }
                else if (strcmp(user_answer, "달력") == 0) {
                    system("cls");
                    set_color(C_INFO);
                    printf("\n[ 달력 조사 ]\n");
                    printf("--------------------------------------------------\n");
                    printf("누군가 오늘 날짜에 펜으로 큼지막하게 동그라미를 쳐두었습니다.\n\n▶ ");
                    set_color(C_ERROR); 
                    printf("[ 2026년 11월 6일 ]\n");
                    set_color(C_INFO);
                    printf("--------------------------------------------------\n");
                    set_color(C_DEFAULT);
                    sound_action();
                    is_event = 1;
                }
                else if (strcmp(user_answer, "단말기") == 0) {
                    system("cls");
                    if (!has_id_card) { 
                        set_color(C_ERROR); printf("\n>> 카드를 태그해야 한다. [사원증]이 필요하다.\n"); set_color(C_DEFAULT); sound_error(); 
                    } 
                    else if (is_door_unlocked) { 
                        set_color(C_INFO); printf("\n>> 출입구 도어락은 이미 해제되어 있습니다.\n"); set_color(C_DEFAULT); sound_action(); 
                    } 
                    else {
                        sound_action();
                        printf("\n>> 사원증 인식.\n\n[ 보안 시스템 단말기 ]\n");
                        printf("오늘 날짜(TODAY)를 8자리로 입력하라. (취소: ESC)\n▶ ");
                        char pw[50];
                        if (input_with_esc(pw, 15)) { 
                            if (strcmp(pw, "20261106") == 0) { 
                                set_color(C_SUCCESS); printf("\n>> 확인 완료. 철컥! 출입구 도어락 잠금이 해제되었습니다!\n"); set_color(C_DEFAULT);
                                sound_success(); is_door_unlocked = 1; 
                            } else { 
                                set_color(C_ERROR); printf("\n>> 삐-빅! 올바르지 않은 날짜입니다.\n"); set_color(C_DEFAULT); sound_error(); 
                            }
                        } else {
                            set_color(C_INFO); printf("\n>> 시스템 조작을 취소합니다.\n"); set_color(C_DEFAULT); sound_action();
                        }
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "도어락") == 0 || strcmp(user_answer, "문") == 0) {
                    system("cls");
                    if (is_door_unlocked) { set_color(C_INFO); printf("\n>> 단말기를 통해 도어락이 해제되어 있다. [복도]로 갈 수 있다.\n"); set_color(C_DEFAULT); sound_action(); }
                    else { set_color(C_ERROR); printf("\n>> 문이 굳게 잠겨있다. 옆에 있는 [단말기]를 조작해야 한다.\n"); set_color(C_DEFAULT); sound_error(); }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "복도") == 0) {
                    if (is_door_unlocked) { current_room = 2; sound_action(); continue; }
                    else { system("cls"); set_color(C_ERROR); printf("\n>> 잠겨있다. 문 옆의 단말기를 살펴봐야 합니다.\n"); set_color(C_DEFAULT); sound_error(); is_event = 1; }
                }
            }
            else if (current_room == 2) { 
                if (strcmp(user_answer, "비밀사무실1") == 0) { current_room = 1; sound_action(); continue; }
                else if (strcmp(user_answer, "탕비실") == 0) { current_room = 3; sound_action(); continue; }
                else if (strcmp(user_answer, "서랍") == 0) {
                    system("cls");
                    if (!has_broken_flash && !has_flash) { 
                        set_color(C_ITEM); printf("\n>> [고장난손전등]을 발견했다!\n"); set_color(C_DEFAULT); 
                        sound_item(); has_broken_flash = 1; 
                    }
                    else { set_color(C_INFO); printf("\n>> 서랍은 텅 비어있다.\n"); set_color(C_DEFAULT); sound_action(); }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "비상문") == 0) {
                    system("cls");
                    if (has_key1) { 
                        set_color(C_SUCCESS);
                        printf("==================================================\n");
                        printf("      축하합니다! 비밀사무실 탈출 성공...!   \n");
                        printf("==================================================\n\n");
                        set_color(C_DEFAULT);
                        sound_success();
                        printf("[ 다음으로 넘어가려면 아무 키나 누르세요... ]\n");
                        getch();

                        system("cls");
                        set_color(C_INFO);
                        printf("\n>> 비상문을 열고 밖으로 나간 줄 알았으나, 그곳은 밖이 아니었습니다.\n");
                        printf(">> 문은 등 뒤에서 철컥 소리를 내며 굳게 잠겨버렸고,\n");
                        printf(">> %s(은)는 숨겨진 공간, [비밀사무실2]로 들어왔습니다...\n\n", player_name);
                        set_color(C_DEFAULT);
                        wait_for_esc();
                        current_room = 4; continue;
                    }
                    else { set_color(C_ERROR); printf("\n>> 자물쇠를 열 [비상문 열쇠]가 필요하다.\n"); set_color(C_DEFAULT); sound_error(); }
                    is_event = 1;
                }
            }
            else if (current_room == 3) { 
                if (strcmp(user_answer, "복도") == 0) { current_room = 2; sound_action(); continue; }
                else if (strcmp(user_answer, "환풍구") == 0) {
                    system("cls");
                    set_color(C_INFO);
                    printf("\n>> 깊고 어두운 환풍구다.\n");
                    printf(">> 안이 전혀 보이지 않는다. 무언가 빛을 비출 물건이 필요할 것 같다.\n");
                    set_color(C_DEFAULT);
                    sound_action();
                    
                    printf("\n [ ★ 현재 가진 아이템 목록 ]\n");
                    set_color(C_ITEM);
                    int empty = 1;
                    if(has_id_card) { printf("  - 사원증\n"); empty = 0; }
                    if(has_battery) { printf("  - 건전지\n"); empty = 0; }
                    if(has_broken_flash) { printf("  - 고장난손전등\n"); empty = 0; }
                    if(has_flash) { printf("  - 손전등\n"); empty = 0; }
                    if(has_key1) { printf("  - 비상문 열쇠\n"); empty = 0; }
                    if(has_remote) { printf("  - 리모컨\n"); empty = 0; }
                    if(has_working_remote) { printf("  - 작동하는리모컨\n"); empty = 0; }
                    if(has_golden_key) { printf("  - 황금열쇠\n"); empty = 0; }
                    if(has_usb) { printf("  - 보안USB\n"); empty = 0; }
                    if(has_masterkey) { printf("  - 마스터키\n"); empty = 0; }
                    if(empty) printf("  (가진 아이템이 없습니다)\n");
                    set_color(C_DEFAULT);

                    char use_item[50];
                    printf("\n▶ 환풍구에 사용할 물건 이름 입력 (취소: 0) : ");
                    scanf("%49s", use_item);
                    clear_input_buffer();

                    if (strcmp(use_item, "0") == 0) {
                        set_color(C_INFO); printf("\n>> 아무것도 사용하지 않았다.\n"); set_color(C_DEFAULT);
                    }
                    else if (strcmp(use_item, "손전등") == 0) {
                        if (has_flash) {
                            if (!has_key1) { 
                                set_color(C_ITEM);
                                printf("\n>> 손전등을 켜서 환풍구 안쪽 깊은 곳을 비추었다!\n");
                                printf(">> 구석에서 반짝이는 [비상문 열쇠]를 발견해 획득했다!\n"); 
                                set_color(C_DEFAULT);
                                sound_item();
                                has_key1 = 1; 
                            }
                            else { set_color(C_INFO); printf("\n>> 이미 안을 확인했다. 먼지만 가득하다.\n"); set_color(C_DEFAULT); sound_action(); }
                        } else {
                            set_color(C_ERROR); printf("\n>> 손전등을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error();
                        }
                    } else {
                        set_color(C_ERROR); printf("\n>> [%s](을)를 사용해보려 했지만 안을 비출 수 없었다.\n", use_item); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "손전등") == 0) {
                    system("cls");
                    if (has_flash) {
                        if (!has_key1) { 
                            set_color(C_ITEM);
                            printf("\n>> 손전등을 켜서 환풍구 안쪽 깊은 곳을 비추었다!\n");
                            printf(">> 구석에서 반짝이는 [비상문 열쇠]를 발견해 획득했다!\n"); 
                            set_color(C_DEFAULT);
                            sound_item();
                            has_key1 = 1; 
                        }
                        else { set_color(C_INFO); printf("\n>> 이미 안을 확인했다. 먼지만 가득하다.\n"); set_color(C_DEFAULT); sound_action(); }
                    } else {
                        set_color(C_ERROR); printf("\n>> 손전등을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
            }
            else if (current_room == 4) {
                if (strcmp(user_answer, "복도") == 0) {
                    system("cls");
                    set_color(C_ERROR); printf("\n>> 등 뒤의 비상문은 이미 굳게 잠겨버려 다시 돌아갈 수 없다!\n"); set_color(C_DEFAULT); sound_error();
                    is_event = 1;
                }
                else if (strcmp(user_answer, "캐비닛") == 0) {
                    system("cls");
                    if (!has_remote && !has_working_remote) {
                        set_color(C_INFO);
                        printf("\n[ 캐비닛 조사 ]\n");
                        printf("--------------------------------------------------\n");
                        printf("낡은 캐비닛 문을 열자 안쪽에 [리모컨]이 놓여 있습니다.\n");
                        printf("하지만 뒷면을 보니 AAA 건전지가 빠져있어 작동하지 않습니다.\n");
                        printf("--------------------------------------------------\n");
                        set_color(C_ITEM);
                        printf(">> [리모컨] 획득!\n");
                        set_color(C_DEFAULT);
                        sound_item();
                        has_remote = 1;
                    } else {
                        set_color(C_INFO); printf("\n>> 캐비닛 안은 텅 비어있다.\n"); set_color(C_DEFAULT); sound_action();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "스크린") == 0) {
                    system("cls");
                    set_color(C_INFO);
                    printf("\n>> 벽면에 커다란 스크린이 걸려있지만 전원이 꺼져 먹먹하다.\n");
                    printf(">> 스크린을 켤 수 있는 무언가가 필요할 것 같다.\n");
                    set_color(C_DEFAULT);
                    sound_action();
                    
                    printf("\n [ ★ 현재 가진 아이템 목록 ]\n");
                    set_color(C_ITEM);
                    int empty = 1;
                    if(has_id_card) { printf("  - 사원증\n"); empty = 0; }
                    if(has_battery) { printf("  - 건전지\n"); empty = 0; }
                    if(has_broken_flash) { printf("  - 고장난손전등\n"); empty = 0; }
                    if(has_flash) { printf("  - 손전등\n"); empty = 0; }
                    if(has_key1) { printf("  - 비상문 열쇠\n"); empty = 0; }
                    if(has_remote) { printf("  - 리모컨\n"); empty = 0; }
                    if(has_working_remote) { printf("  - 작동하는리모컨\n"); empty = 0; }
                    if(has_golden_key) { printf("  - 황금열쇠\n"); empty = 0; }
                    if(has_usb) { printf("  - 보안USB\n"); empty = 0; }
                    if(has_masterkey) { printf("  - 마스터키\n"); empty = 0; }
                    if(empty) printf("  (가진 아이템이 없습니다)\n");
                    set_color(C_DEFAULT);

                    char use_item[50];
                    printf("\n▶ 스크린에 사용할 물건 이름 입력 (취소: 0) : ");
                    scanf("%49s", use_item);
                    clear_input_buffer();

                    if (strcmp(use_item, "0") == 0) {
                        set_color(C_INFO); printf("\n>> 아무것도 사용하지 않았다.\n"); set_color(C_DEFAULT);
                    }
                    else if (strcmp(use_item, "작동하는리모컨") == 0) {
                        if (has_working_remote) {
                            set_color(C_SUCCESS);
                            printf("\n>> 리모컨을 스크린 쪽으로 향하고 전원 버튼을 꾹 누르자...\n");
                            printf(">> 삐빅! 소리와 함께 낡은 스크린이 켜졌습니다!\n\n");
                            set_color(C_ITEM);
                            printf("   [ 행운의 금고 비밀번호 : 7777 ]   \n\n");
                            set_color(C_INFO);
                            printf(">> 스크린에 나타난 번호를 확인했습니다.\n");
                            set_color(C_DEFAULT);
                            sound_success();
                        } else {
                            set_color(C_ERROR); printf("\n>> 해당 아이템을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error();
                        }
                    } 
                    else if (strcmp(use_item, "리모컨") == 0) {
                        if (has_remote) {
                            set_color(C_ERROR); printf("\n>> 리모컨 버튼을 눌러보지만 아무 반응이 없습니다. 건전지가 필요합니다.\n"); set_color(C_DEFAULT); sound_error();
                        } else {
                            set_color(C_ERROR); printf("\n>> 해당 아이템을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error();
                        }
                    }
                    else {
                        set_color(C_ERROR); printf("\n>> [%s](을)를 사용해보려 했지만 스크린은 켜지지 않았다.\n", use_item); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "리모컨") == 0 || strcmp(user_answer, "작동하는리모컨") == 0) {
                    system("cls");
                    if (has_working_remote) {
                        set_color(C_SUCCESS);
                        printf("\n>> 리모컨의 전원 버튼을 꾹 누르자...\n");
                        printf(">> 삐빅! 소리와 함께 벽면의 낡은 스크린이 켜졌습니다!\n\n");
                        set_color(C_ITEM);
                        printf("   [ 행운의 금고 비밀번호 : 7777 ]   \n\n");
                        set_color(C_INFO);
                        printf(">> 스크린에 나타난 번호를 확인했습니다.\n");
                        set_color(C_DEFAULT);
                        sound_success();
                    } else if (has_remote) {
                        set_color(C_ERROR); printf("\n>> 버튼을 눌러보지만 아무 반응이 없습니다. AAA 건전지가 필요합니다.\n"); set_color(C_DEFAULT); sound_error();
                    } else {
                        set_color(C_ERROR); printf("\n>> 리모컨을 가지고 있지 않습니다.\n"); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "금고") == 0) {
                    system("cls");
                    if (has_golden_key) {
                        set_color(C_INFO); printf("\n>> 금고는 이미 열려 있고 안은 텅 비어 있다.\n"); set_color(C_DEFAULT); sound_action();
                    } else {
                        sound_action();
                        char pw[50]; 
                        printf("\n[ 다이얼 금고 해제 ]\n");
                        printf("다이얼 4자리 숫자를 입력하세요. (취소: ESC)\n▶ "); 
                        
                        if (input_with_esc(pw, 15)) { 
                            if (strcmp(pw, "7777") == 0) { 
                                set_color(C_ITEM); printf("\n>> 찰칵! 금고 문이 무겁게 열리며 [황금열쇠]를 획득했습니다!\n"); set_color(C_DEFAULT);
                                sound_item(); has_golden_key = 1; 
                            }
                            else { set_color(C_ERROR); printf("\n>> 다이얼이 돌아가지 않습니다. 틀린 것 같습니다.\n"); set_color(C_DEFAULT); sound_error(); }
                        } else {
                            set_color(C_INFO); printf("\n>> 입력을 취소하고 돌아갑니다.\n"); set_color(C_DEFAULT); sound_action();
                        }
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "철문") == 0) {
                    system("cls");
                    if (has_golden_key) { 
                        set_color(C_SUCCESS);
                        printf("==================================================\n");
                        printf("          철문 개방! 이번엔 진짜 탈출...?!       \n");
                        printf("==================================================\n\n");
                        set_color(C_DEFAULT);
                        sound_success();
                        printf("[ 다음으로 넘어가려면 아무 키나 누르세요... ]\n");
                        getch();

                        system("cls");
                        set_color(C_INFO);
                        printf("\n>> 황금열쇠를 꽂고 돌리자 철문이 굉음을 내며 열립니다!\n");
                        printf(">> 하지만 그곳은 강렬한 백색 조명이 켜진 마지막 통제 구역,\n");
                        printf(">> [비밀사무실3] 이었습니다.\n\n");
                        set_color(C_DEFAULT);
                        wait_for_esc();
                        current_room = 5; continue;
                    }
                    else {
                        set_color(C_ERROR); printf("\n>> 거대한 자물쇠가 채워진 철문이다.\n>> 이 자물쇠를 열 [황금열쇠]가 필요하다.\n"); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
            }
            else if (current_room == 5) {
                if (strcmp(user_answer, "철문") == 0 || strcmp(user_answer, "비밀사무실2") == 0) {
                    system("cls"); set_color(C_ERROR); printf("\n>> 육중한 철문은 이미 굳게 닫혀버려 돌아갈 수 없다.\n"); set_color(C_DEFAULT); sound_error(); is_event = 1;
                }
                else if (strcmp(user_answer, "서류가방") == 0) {
                    system("cls");
                    if (!has_usb) { set_color(C_ITEM); printf("\n>> 가방 안에서 [보안USB]를 발견했습니다!\n"); set_color(C_DEFAULT); sound_item(); has_usb = 1; } 
                    else { set_color(C_INFO); printf("\n>> 가방 안은 이제 텅 비어 있습니다.\n"); set_color(C_DEFAULT); sound_action(); }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "컴퓨터") == 0) {
                    system("cls");
                    if (has_usb) {
                        set_color(C_SUCCESS);
                        printf("\n>> 본체에 [보안USB]를 삽입했습니다.\n");
                        printf("--------------------------------------------------\n");
                        printf("  [ 접근 권한 승인 완료 ]\n");
                        set_color(C_ITEM);
                        printf("  * 지상행 엘리베이터 비상 가동 암호 : 1024 *\n");
                        set_color(C_SUCCESS);
                        printf("--------------------------------------------------\n");
                        set_color(C_DEFAULT);
                        sound_success();
                    } else {
                        set_color(C_ERROR); printf("\n>> 모니터에는 '보안 키(USB)를 삽입하세요'라는 경고 문구만 깜빡거립니다.\n"); set_color(C_DEFAULT); sound_error();
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "엘리베이터") == 0) {
                    system("cls");
                    sound_action();
                    char pw[50]; 
                    printf("\n[ 지상행 엘리베이터 단말기 ]\n");
                    printf("4자리 가동 암호를 입력하세요. (취소: ESC)\n▶ "); 
                    
                    if (input_with_esc(pw, 15)) { 
                        if (strcmp(pw, "1024") == 0) { 
                            set_color(C_SUCCESS);
                            printf("\n>> 삐빅! [인증 완료]\n"); 
                            printf(">> 덜컹거리는 소리와 함께 엘리베이터 문이 열립니다.\n"); 
                            printf(">> 이번에야말로... 진짜로 지상으로 올라갑니다!\n"); 
                            set_color(C_DEFAULT);
                            sound_success(); 
                            is_escaped = 1; 
                        }
                        else { set_color(C_ERROR); printf("\n>> 삐-빅! 잘못된 암호입니다.\n"); set_color(C_DEFAULT); sound_error(); }
                    } else { set_color(C_INFO); printf("\n>> 입력을 취소하고 돌아갑니다.\n"); set_color(C_DEFAULT); sound_action(); }
                    is_event = 1;
                }
            }
        }
        else if (episode == 2) {
            if (current_room == 1) {
                if (strcmp(user_answer, "도어락") == 0 || strcmp(user_answer, "문") == 0) {
                    system("cls");
                    if (is_door_unlocked) { set_color(C_INFO); printf("\n>> 이미 열려있다. [원장실]로 갈 수 있다.\n"); set_color(C_DEFAULT); sound_action(); }
                    else {
                        sound_action();
                        char pw[50]; 
                        printf("\n[ 도어락 비밀번호 해제 ]\n");
                        printf("도어락 4자리 비밀번호를 입력하세요. (취소: ESC)\n▶ "); 
                        
                        if (input_with_esc(pw, 15)) { 
                            if (strcmp(pw, "4321") == 0) { set_color(C_SUCCESS); printf("\n>> 철컥! 도어락 잠금이 해제되었습니다!\n"); set_color(C_DEFAULT); sound_success(); is_door_unlocked = 1; }
                            else { set_color(C_ERROR); printf("\n>> 삐-빅! 비밀번호가 틀렸습니다.\n"); set_color(C_DEFAULT); sound_error(); }
                        } else { set_color(C_INFO); printf("\n>> 입력을 취소하고 돌아갑니다.\n"); set_color(C_DEFAULT); sound_action(); }
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "원장실") == 0) {
                    if (is_door_unlocked) { current_room = 2; sound_action(); continue; }
                    else { system("cls"); set_color(C_ERROR); printf("\n>> 도어락이 굳게 잠겨있다.\n"); set_color(C_DEFAULT); sound_error(); is_event = 1; }
                }
            }
            else if (current_room == 2) {
                if (strcmp(user_answer, "상담실") == 0) { current_room = 1; sound_action(); continue; }
                else if (strcmp(user_answer, "상자") == 0) {
                    system("cls");
                    if (has_masterkey) { set_color(C_INFO); printf("\n>> 상자는 이미 열려있다.\n"); set_color(C_DEFAULT); sound_action(); } 
                    else {
                        sound_action();
                        char word[50]; 
                        printf("\n[ 상자 자물쇠 해제 ]\n");
                        printf("자물쇠 단어(한글)를 입력하세요. (취소: 0)\n▶ "); 
                        scanf("%49s", word); clear_input_buffer(); 
                        
                        if (strcmp(word, "0") == 0) { set_color(C_INFO); printf("\n>> 입력을 취소하고 돌아갑니다.\n"); set_color(C_DEFAULT); sound_action(); }
                        else if (strcmp(word, "일기장") == 0) { set_color(C_ITEM); printf("\n>> 상자가 열리며 [마스터키] 획득!\n"); set_color(C_DEFAULT); sound_item(); has_masterkey = 1; } 
                        else { set_color(C_ERROR); printf("\n>> 자물쇠는 꿈쩍도 하지 않는다.\n"); set_color(C_DEFAULT); sound_error(); }
                    }
                    is_event = 1;
                }
                else if (strcmp(user_answer, "비상구") == 0) {
                    system("cls");
                    if (has_masterkey) { set_color(C_SUCCESS); printf("\n>> 마스터키로 비상구를 열고 탈출!!!\n"); set_color(C_DEFAULT); sound_success(); is_escaped = 1; }
                    else { set_color(C_ERROR); printf("\n>> 탈출하려면 [마스터키]가 필요하다.\n"); set_color(C_DEFAULT); sound_error(); }
                    is_event = 1;
                }
            }
        }

        if (is_event) { wait_for_esc(); } 
        else {
            sprintf(obj_file, "%s_%s.txt", room_names[current_room], user_answer);
            FILE* fp = fopen(obj_file, "r");
            if (fp != NULL) {
                system("cls");
                sound_action();
                set_color(C_INFO);
                printf("\n[ %s 조사 ]\n", user_answer);
                printf("--------------------------------------------------\n");
                set_color(C_DEFAULT);
                char line[256];
                
                while (fgets(line, sizeof(line), fp) != NULL) {
                    char temp[1024] = {0};
                    char *pos = line;
                    char *match;
                    
                    while ((match = strstr(pos, "당신")) != NULL) {
                        strncat(temp, pos, match - pos);
                        strcat(temp, player_name);
                        pos = match + strlen("당신");
                    }
                    strcat(temp, pos);

                    if (difficulty_mode == 2 || (difficulty_mode == 3 && hint_mode == 1)) { 
                        printf("%s", temp); 
                    } else {
                        for (int k = 0; temp[k] != '\0'; k++) {
                            if (temp[k] != '[' && temp[k] != ']') putchar(temp[k]);
                        }
                    }
                }
                set_color(C_INFO);
                printf("\n--------------------------------------------------\n");
                set_color(C_DEFAULT);
                fclose(fp); wait_for_esc();
            } else {
                system("cls");
                set_color(C_ERROR);
                printf("\n>> '%s' - 없는 행동이거나, 여긴 없습니다.\n", user_answer);
                set_color(C_DEFAULT);
                sound_error();
                wait_for_esc();
            }
        }
    }
    
    int final_time = acc_time + (int)difftime(time(NULL), start_time);
    if (episode == 1) ep1_time = final_time;
    else if (episode == 2) ep2_time = final_time;

    animate_clear_screen(episode, final_time); 
    
    return 1; 
}

void display_scene_from_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    char line[256];
    if (fp == NULL) { 
        set_color(C_ERROR);
        printf("[시스템] '%s' 파일이 없습니다.\n", filename); 
        set_color(C_DEFAULT);
        return; 
    }
    
    while (fgets(line, sizeof(line), fp) != NULL) {
        char temp[1024] = {0};
        char *pos = line;
        char *match;
        
        while ((match = strstr(pos, "당신")) != NULL) {
            strncat(temp, pos, match - pos); 
            strcat(temp, player_name);       
            pos = match + strlen("당신");    
        }
        strcat(temp, pos); 

        if (difficulty_mode == 2 || (difficulty_mode == 3 && hint_mode == 1)) { 
            printf("%s", temp); 
        } 
        else {
            for (int i = 0; temp[i] != '\0'; i++) {
                if (temp[i] != '[' && temp[i] != ']') putchar(temp[i]);
            }
        }
    }
    fclose(fp);
}
