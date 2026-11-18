#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <ctime>
#include <cstdlib>

using namespace std;

// --- 전역 상태 제어 (C++98 호환 volatile 사용) ---
volatile bool isBgmPlaying = true;
volatile bool runBgmThread = true;
volatile bool pauseBgmForSfx = false;

// --- 전역 확률 테이블 ---
const int sucR[25] = {0, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5}; 
const int desR[25] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 15, 20, 25, 30, 25, 25, 30, 30, 30};
const int dropR[25]= {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20, 25, 30, 35, 40};

// --- 구조체 정의 ---
struct Sword {
    int level;
    bool isMutant;
    double mult;
};

struct Player {
    Sword equip;
    vector<Sword> inv;
    int destroyed[25]; 
    int materials[25]; 
    int gold;
    int maxLevel;
    int dropPrevent;     
    int successBoost;    
    int pickaxe;         
    time_t startTime;
} p;

// --- 데이터 초기화 유틸 ---
void initPlayer() {
    p.equip.level = 1;
    p.equip.isMutant = false;
    p.equip.mult = 1.0;
    p.gold = 1000;
    p.maxLevel = 1;
    p.dropPrevent = 0;
    p.successBoost = 0;
    p.pickaxe = 0;
    for(int i=0; i<25; i++) {
        p.destroyed[i] = 0;
        p.materials[i] = 0;
    }
}

// --- 안전한 입력 함수 (C++98 호환) ---
int getIntInput() {
    string s;
    cin >> s;
    return atoi(s.c_str());
}

// --- 사운드 시스템 (Windows API 멀티스레드) ---
DWORD WINAPI BGM_Thread(LPVOID lpParam) {
    int melody[] = { 261, 329, 392, 523, 392, 329 };
    while (runBgmThread) {
        for (int i = 0; i < 6; i++) {
            if (!runBgmThread) break;
            // BGM이 켜져있고, 효과음 재생 중이 아닐 때만 멜로디 재생
            if (isBgmPlaying && !pauseBgmForSfx) {
                Beep(melody[i], 300);
                Sleep(50);
            } else {
                Sleep(350);
            }
        }
    }
    return 0;
}

void playSound(int type) {
    // 효과음은 BGM 끄기와 상관없이 무조건 들리게 변경!
    pauseBgmForSfx = true; // 멜로디 임시 정지
    switch(type) {
        case 1: Beep(1000, 100); Beep(1200, 150); Beep(1500, 200); break; // 강화 성공
        case 2: Beep(300, 400); break;                                    // 강화 실패
        case 3: Beep(150, 300); Beep(100, 400); break;                    // 파괴
        case 4: Beep(800, 100); Beep(900, 100); Beep(1000, 200); break;   // 조합/복구
        case 5: Beep(600, 150); Beep(400, 200); break;                    // 분해
        case 6: Beep(200, 500); break;                                    // 하락
    }
    pauseBgmForSfx = false; // 멜로디 재개
}

// --- 데이터 저장/불러오기 ---
void saveGame() {
    ofstream out("save_game.txt");
    out << p.equip.level << " " << p.equip.isMutant << " " << p.equip.mult << "\n";
    out << p.gold << " " << p.maxLevel << " " << p.dropPrevent << " " << p.successBoost << " " << p.pickaxe << "\n";
    out << p.inv.size() << "\n";
    for (size_t i = 0; i < p.inv.size(); i++) {
        out << p.inv[i].level << " " << p.inv[i].isMutant << " " << p.inv[i].mult << "\n";
    }
    for (int i = 1; i <= 20; i++) out << p.destroyed[i] << " "; out << "\n";
    for (int i = 1; i <= 20; i++) out << p.materials[i] << " "; out << "\n";
    out.close();
}

bool loadGame() {
    ifstream in("save_game.txt");
    if (!in) return false;
    in >> p.equip.level >> p.equip.isMutant >> p.equip.mult;
    in >> p.gold >> p.maxLevel >> p.dropPrevent >> p.successBoost >> p.pickaxe;
    int invSize; in >> invSize;
    p.inv.clear();
    for (int i = 0; i < invSize; i++) {
        Sword s; in >> s.level >> s.isMutant >> s.mult;
        p.inv.push_back(s);
    }
    for (int i = 1; i <= 20; i++) in >> p.destroyed[i];
    for (int i = 1; i <= 20; i++) in >> p.materials[i];
    return true;
}

// --- 밸런스 조정 함수 ---
int getEnhanceCost(int level) { 
    // 강화 비용을 대폭 증가시킴 (예: 1단계 500G, 10단계 50000G)
    return level * level * 500; 
}

int getSellPrice(Sword s) {
    if (s.level == 1) return 0; // 1단계는 판매 불가 처리
    // 판매 금액 대폭 증가 (고단계 검을 팔면 큰 이득)
    int base = s.level * s.level * 400; 
    if (s.isMutant) base = (int)(base * s.mult);
    return base;
}

void equipLvl1() {
    p.equip.level = 1;
    p.equip.isMutant = false;
    p.equip.mult = 1.0;
}

void Ending() {
    system("cls");
    time_t endTime = time(0);
    double elapsed = difftime(endTime, p.startTime);
    cout << "\n\n\n\t\t ==========================================\n";
    cout << "\t\t [ 축하합니다! 전설의 20단계 검을 완성했습니다! ]\n";
    cout << "\t\t ==========================================\n";
    cout << "\n\t\t 총 소요 시간: " << (int)elapsed / 60 << "분 " << (int)elapsed % 60 << "초\n";
    cout << "\n\n 아무 키나 누르시면 게임이 완전히 종료됩니다...";
    _getch();
    runBgmThread = false;
    exit(0);
}

// --- 메인 인게임 반복문 ---
void EnhanceGame() {
    while (true) {
        system("cls");
        cout << "========================================================\n";
        cout << " [ 장착 무기 ]: " << p.equip.level << "단계 검 ";
        if (p.equip.isMutant) cout << "(변종! 가치 x" << p.equip.mult << ") ";
        cout << "\n [ 소지 골드 ]: " << p.gold << " G   |   [ 최고 달성 단계 ]: " << p.maxLevel << " 단계\n";
        cout << " [ 확률증가권 ]: " << p.successBoost << "장  |   [ 하락방지권 ]: " << p.dropPrevent << "장\n";
        cout << " [ 곡괭이 ]: " << (p.pickaxe == 0 ? "없음" : (p.pickaxe == 1 ? "철 곡괭이" : "다이아몬드 곡괭이")) << "\n";
        cout << "--------------------------------------------------------\n";
        
        // --- 강화 확률 및 정보 UI 출력 ---
        if (p.equip.level < 20) {
            int s = sucR[p.equip.level];
            int d = desR[p.equip.level];
            int dr = dropR[p.equip.level];
            int failR = 100 - s - d - dr;
            
            cout << " [ 다음 강화 확률 ] 성공: " << s << "% | 실패(유지): " << failR << "%";
            if (d > 0) cout << " | 파괴: " << d << "%";
            if (dr > 0) cout << " | 하락: " << dr << "%";
            cout << "\n";
            
            cout << " [ 강화 필요 골드 ] " << getEnhanceCost(p.equip.level) << " G";
            if (p.equip.level >= 15) {
                cout << " (필요 재료: " << p.equip.level << "단계 분해 재료 1개 - 현재 " << p.materials[p.equip.level] << "개)\n";
            } else {
                cout << "\n";
            }
        } else {
            cout << " [ 최고 단계에 도달했습니다! 더 이상 강화할 수 없습니다. ]\n";
        }
        cout << "========================================================\n";
        cout << " 1. 검 강화하기\n 2. 인벤토리 (보관/스왑/조합)\n 3. 파괴된 검 복구\n 4. 상점 가기\n 5. 광산 가기 (골드 벌기)\n 6. 현재 검 분해\n 7. 현재 검 판매\n 8. 게임 저장하기\n 9. 메인 메뉴로 돌아가기\n";
        cout << "========================================================\n";
        cout << " 선택: ";
        
        char ch = _getch();
        
        if (ch == '9') break;
        if (ch == '8') { 
            saveGame(); 
            cout << "\n[알림] 성공적으로 저장되었습니다!"; 
            Sleep(1000); 
            continue; 
        }

        // 1. 강화하기
        if (ch == '1') {
            if (p.equip.level >= 20) continue;
            int cost = getEnhanceCost(p.equip.level);
            if (p.gold < cost) { cout << "\n[알림] 골드가 부족합니다. (비용: " << cost << "G)"; Sleep(1000); continue; }
            
            bool useProtect = false, useBoost = false;
            
            if (p.equip.level >= 15) {
                if (p.materials[p.equip.level] < 1) { 
                    cout << "\n[알림] " << p.equip.level << "단계 분해 재료가 1개 부족합니다!"; 
                    Sleep(1500); continue; 
                }
                if (p.dropPrevent > 0) {
                    cout << "\n하락 방지권을 사용하시겠습니까? (y/n): ";
                    if (_getch() == 'y') { useProtect = true; p.dropPrevent--; }
                }
                if (p.successBoost > 0) {
                    cout << "\n확률 증가권(+15%)을 사용하시겠습니까? (y/n): ";
                    if (_getch() == 'y') { useBoost = true; p.successBoost--; }
                }
                p.materials[p.equip.level]--;
            }

            p.gold -= cost;
            cout << "\n\n[" << cost << "G 소모] 강화를 시작합니다...\n"; 
            Sleep(800);

            int s = sucR[p.equip.level] + (useBoost ? 15 : 0);
            int d = desR[p.equip.level];
            int dr = dropR[p.equip.level];
            if (useProtect) dr = 0; // 방지권 사용 시 하락 0% 처리

            int roll = rand() % 100 + 1;

            if (roll <= s) {
                playSound(1);
                p.equip.level++;
                p.equip.isMutant = false;
                
                if (rand() % 1000 < 5) { // 0.5% 확률
                    p.equip.isMutant = true;
                    p.equip.mult = 1.1 + (rand() % 20) / 10.0;
                    cout << "[대성공] " << p.equip.level << "단계 '변종 검'을 획득했습니다!! (판매 가치 " << p.equip.mult << "배)\n";
                } else {
                    cout << "[성공] " << p.equip.level << "단계 검 강화에 성공했습니다!\n";
                }

                if (p.equip.level > p.maxLevel) p.maxLevel = p.equip.level;
                if (p.equip.level == 20) Ending();
            }
            else if (roll <= s + d && p.equip.level >= 10) {
                playSound(3);
                cout << "[파괴] 강화 실패로 검이 산산조각 났습니다...\n";
                p.destroyed[p.equip.level]++;
                equipLvl1();
            }
            else if (roll <= s + d + dr && p.equip.level >= 15) {
                playSound(6);
                cout << "[하락] 강화 실패로 검의 등급이 1단계 하락했습니다...\n";
                p.equip.level--;
            }
            else {
                playSound(2);
                cout << "[실패] 강화에 실패했습니다. (검 유지)\n";
            }
            cout << "아무 키나 누르세요..."; _getch();
        }
        
        // 2. 인벤토리
        else if (ch == '2') { 
            while(true) {
                system("cls");
                cout << "[ 인벤토리 (총 " << (int)p.inv.size() << "개) ]\n";
                for (size_t i = 0; i < p.inv.size(); i++) {
                    cout << i + 1 << ". " << p.inv[i].level << "단계 검 ";
                    if (p.inv[i].isMutant) cout << "(변종)";
                    cout << "\n";
                }
                cout << "\n 1. 현재 장착검 보관\n 2. 인벤토리 검과 스왑(교체)\n 3. 동일레벨 검 조합 (10단계 이상)\n 4. 뒤로가기\n 선택: ";
                char i_ch = _getch();
                
                if (i_ch == '4') break;
                if (i_ch == '1') {
                    p.inv.push_back(p.equip);
                    equipLvl1();
                    cout << "\n[알림] 보관 완료! 1단계 기본 검이 장착되었습니다."; 
                    Sleep(1200); break;
                }
                else if (i_ch == '2' && !p.inv.empty()) {
                    cout << "\n교체할 검의 번호를 입력하세요: ";
                    int idx = getIntInput() - 1;
                    if (idx >= 0 && idx < (int)p.inv.size()) {
                        Sword temp = p.equip;
                        p.equip = p.inv[idx];
                        p.inv[idx] = temp;
                        cout << "\n[알림] 무기가 교체되었습니다!";
                    } else {
                        cout << "\n[오류] 잘못된 번호입니다.";
                    }
                    Sleep(1000);
                }
                else if (i_ch == '3') {
                    cout << "\n조합할 검의 단계를 입력하세요 (10단계 이상): ";
                    int lvl = getIntInput();
                    if (lvl < 10) { cout << "\n[오류] 10단계 이상만 조합 가능합니다."; Sleep(1000); continue; }
                    
                    vector<int> idxs;
                    for (size_t i = 0; i < p.inv.size(); i++) {
                        if (p.inv[i].level == lvl && !p.inv[i].isMutant) idxs.push_back(i);
                    }
                    
                    if (idxs.size() >= 2) {
                        playSound(4);
                        p.inv.erase(p.inv.begin() + idxs[1]);
                        p.inv.erase(p.inv.begin() + idxs[0]);
                        
                        cout << "\n조합 중입니다...\n"; Sleep(1000);
                        if(rand() % 100 < 80) { // 80% 조합 성공
                            int jump = rand() % 3 + 1;
                            int newLvl = lvl + jump;
                            if(newLvl > p.maxLevel) newLvl = p.maxLevel; 
                            
                            Sword newSword;
                            newSword.level = newLvl;
                            newSword.isMutant = false;
                            newSword.mult = 1.0;
                            p.inv.push_back(newSword);
                            cout << "[조합 성공!] " << newLvl << "단계 검으로 도약했습니다!!";
                        } else {
                            playSound(3);
                            cout << "[조합 실패] 검이 파괴되어 사라졌습니다...";
                            p.destroyed[lvl] += 2;
                        }
                    } else { 
                        cout << "\n[오류] 인벤토리에 동일 레벨(일반) 검이 2개 이상 필요합니다."; 
                    }
                    Sleep(1500);
                }
            }
        }
        
        // 3. 복구
        else if (ch == '3') { 
            system("cls");
            cout << "[ 파괴된 검 현황 ]\n";
            for(int i = 10; i <= 20; i++) {
                if (p.destroyed[i] > 0) cout << i << "단계: " << p.destroyed[i] << "개 (5개당 1개 복구가능)\n";
            }
            cout << "\n복구할 단계를 입력하세요 (취소 0): "; 
            int l = getIntInput();
            if (l >= 10 && l <= 20) {
                if(p.destroyed[l] >= 5) {
                    playSound(4);
                    p.destroyed[l] -= 5;
                    Sword recSword;
                    recSword.level = l;
                    recSword.isMutant = false;
                    recSword.mult = 1.0;
                    p.inv.push_back(recSword);
                    cout << "\n[복구 성공!] 파괴된 검 5개를 조합하여 온전한 " << l << "단계 검을 인벤토리에 넣었습니다.";
                } else cout << "\n[오류] 파괴된 검이 부족합니다. (필요: 5, 보유: " << p.destroyed[l] << ")";
                Sleep(1500);
            }
        }
        
        // 4. 상점
        else if (ch == '4') { 
            while(true) {
                system("cls");
                int limit = p.maxLevel - 1;
                if(limit < 1) limit = 1;
                
                cout << "==== [ 상 점 ] ==== (소지 골드: " << p.gold << " G)\n";
                cout << " 1. 원하는 단계 검 구매 (최대 " << limit << "단계까지 구매 가능)\n";
                cout << " 2. 하락 방지권 구매 (1000G / 15단계 이상 전용)\n";
                cout << " 3. 확률 증가권 구매 (1000G / 15단계 이상 전용)\n";
                cout << " 4. 곡괭이 구매 (광산용)\n";
                cout << " 5. 나가기\n 선택: ";
                char s_ch = _getch();
                
                if (s_ch == '5') break;
                if (s_ch == '1') {
                    cout << "\n구매할 단계를 입력하세요 (1 ~ " << limit << "): "; 
                    int bl = getIntInput();
                    if(bl >= 1 && bl <= limit) {
                        int bcost = bl * bl * 500; // 강화비용과 동일한 밸런스로 수정
                        if (p.gold >= bcost) { 
                            p.gold -= bcost; 
                            Sword buySword;
                            buySword.level = bl; buySword.isMutant = false; buySword.mult = 1.0;
                            p.inv.push_back(buySword); 
                            cout << "\n[구매 완료] -" << bcost << "G"; 
                        } else cout << "\n[골드 부족] 비용: " << bcost << "G";
                    } else cout << "\n[오류] 구매할 수 없는 단계입니다.";
                    Sleep(1200);
                }
                else if (s_ch == '2') {
                    if (p.gold >= 1000) { p.gold -= 1000; p.dropPrevent++; cout << "\n[구매 완료]"; }
                    else cout << "\n[골드 부족]"; Sleep(1000);
                }
                else if (s_ch == '3') {
                    if (p.gold >= 1000) { p.gold -= 1000; p.successBoost++; cout << "\n[구매 완료]"; }
                    else cout << "\n[골드 부족]"; Sleep(1000);
                }
                else if (s_ch == '4') {
                    cout << "\n 1. 철 곡괭이 (500G / 광산 5키)   2. 다이아 곡괭이 (2000G / 광산 3키)\n 선택: ";
                    char pc = _getch();
                    if(pc == '1' && p.gold >= 500) { p.gold -= 500; p.pickaxe = 1; cout << "\n[구매 완료]"; }
                    else if(pc == '2' && p.gold >= 2000) { p.gold -= 2000; p.pickaxe = 2; cout << "\n[구매 완료]"; }
                    else cout << "\n[골드 부족 혹은 잘못된 입력]"; Sleep(1000);
                }
            }
        }
        
        // 5. 광산
        else if (ch == '5') { 
            if(p.pickaxe == 0) {
                cout << "\n[접근 불가] 상점에서 곡괭이를 먼저 구매해야 합니다!"; Sleep(1500); continue;
            }
            system("cls");
            string keys = "asdf";
            int req = (p.pickaxe == 1) ? 5 : 3; 
            string target = "";
            for(int i = 0; i < req; i++) target += keys[rand() % 4];
            
            cout << "==== [ 광산 (LV: " << p.maxLevel << ") ] ====\n";
            cout << "아래의 문자를 2초 안에 빠르게 입력하세요!\n\n";
            cout << "  목표 문자: " << target << "\n\n";
            cout << "  입력: ";
            
            string input = "";
            time_t st = time(0);
            while(input.length() < (size_t)req) {
                if(_kbhit()) { 
                    char k = _getch(); 
                    cout << k; 
                    input += k; 
                }
                if(time(0) - st > 2) { 
                    cout << "\n\n[실패] 시간 초과! 채굴에 실패했습니다."; 
                    Sleep(1500); break; 
                }
            }
            
            if(input == target) {
                int rnd = rand() % 100;
                int earn = 0; string ore = "";
                int t1 = 40 - p.maxLevel; if (t1 < 0) t1 = 0;
                int t2 = 70 - p.maxLevel/2; if (t2 < 0) t2 = 0;
                
                if (rnd < t1) { earn = 50 + p.maxLevel*10; ore = "석탄"; }
                else if (rnd < t2) { earn = 100 + p.maxLevel*20; ore = "철"; }
                else if (rnd < 85) { earn = 200 + p.maxLevel*30; ore = "은"; }
                else if (rnd < 95) { earn = 500 + p.maxLevel*50; ore = "금"; }
                else { earn = 1000 + p.maxLevel*100; ore = "다이아몬드"; }
                
                cout << "\n\n[채굴 성공!] " << ore << " 광석을 캐냈습니다! (+" << earn << " G)";
                p.gold += earn;
                Sleep(1500);
            } else if (input.length() == (size_t)req && input != target) {
                cout << "\n\n[실패] 잘못 입력했습니다."; Sleep(1500);
            }
        }
        
        // 6. 분해
        else if (ch == '6') { 
            if (p.equip.level < 1) continue;
            playSound(5);
            p.materials[p.equip.level]++;
            cout << "\n[분해 완료] 검을 분해하여 '" << p.equip.level << "단계 강화 재료' 1개를 얻었습니다.";
            equipLvl1();
            Sleep(1500);
        }
        
        // 7. 판매
        else if (ch == '7') { 
            if (p.equip.level == 1) { // 1단계 판매 불가 처리
                cout << "\n[알림] 1단계 기본 검은 판매할 수 없습니다!";
                Sleep(1500);
                continue;
            }
            int val = getSellPrice(p.equip);
            p.gold += val;
            cout << "\n[판매 완료] 장착 중인 검을 " << val << " G에 판매했습니다.";
            equipLvl1();
            Sleep(1200);
        }
    }
}

// --- 메인 메뉴 UI ---
void MainMenu() {
    int menuIdx = 0;
    while (true) {
        system("cls");
        cout << "\n\n\n\t\t [ 검 강화 게임 메인 메뉴 ]\n\n";
        
        string menus[] = {"게임 시작", "이어 하기 (불러오기)", "게임 설명", (isBgmPlaying ? "BGM 끄기 (효과음은 유지)" : "BGM 켜기"), "게임 종료"};
        
        for (int i = 0; i < 5; i++) {
            if (i == menuIdx) cout << "\t\t   ▶ " << menus[i] << " ◀\n";
            else cout << "\t\t      " << menus[i] << "    \n";
        }
        
        char ch = _getch();
        if (ch == -32 || ch == 224) { 
            ch = _getch();
            if (ch == 72 && menuIdx > 0) menuIdx--; // Up
            if (ch == 80 && menuIdx < 4) menuIdx++; // Down
        }
        else if (ch == 13) { 
            if (menuIdx == 0) { 
                p.startTime = time(0); 
                EnhanceGame(); 
            }
            else if (menuIdx == 1) { 
                if (loadGame()) { 
                    cout << "\n\t\t 데이터를 성공적으로 불러왔습니다!"; 
                    Sleep(1000); 
                    p.startTime = time(0);
                    EnhanceGame(); 
                }
                else { cout << "\n\t\t 저장된 파일이 존재하지 않습니다."; Sleep(1500); }
            }
            else if (menuIdx == 2) {
                system("cls");
                cout << " =================== [ 게임 설명 ] ===================\n\n";
                cout << " - 목표: 전설의 20단계 검을 만들면 게임 클리어!\n";
                cout << " - 파괴: 10단계 검부터 강화 실패 시 파괴될 수 있습니다.\n";
                cout << " - 하락: 15단계부터 강화 시 해당 단계의 '분해 재료'가 필요하며, 실패 시 하락합니다.\n";
                cout << " - 복구: 파괴된 동일 단계 검 5개를 인벤토리에서 하나로 복구할 수 있습니다.\n";
                cout << " - 조합: 인벤토리에서 동일 단계 검(10단계 이상) 두 개를 합쳐 확률적으로 도약합니다.\n";
                cout << " - 광산: ASDF 미니게임을 통해 골드를 벌 수 있으며 곡괭이가 필요합니다.\n";
                cout << " - 변종: 강화 성공 시 0.5% 확률로 판매 가치가 높은 변종 검이 탄생합니다.\n\n";
                cout << " 아무 키나 누르시면 돌아갑니다...";
                _getch();
            }
            else if (menuIdx == 3) {
                isBgmPlaying = !isBgmPlaying; // BGM 토글
            }
            else if (menuIdx == 4) {
                runBgmThread = false; 
                exit(0);
            }
        }
    }
}

int main() {
    srand((unsigned int)time(0));
    initPlayer();
    
    // Windows API 방식 스레드 생성 (C++98 호환)
    CreateThread(NULL, 0, BGM_Thread, NULL, 0, NULL);

    system("cls");
    cout << "\n\n\n\n\n\t\t ==================================\n";
    cout << "\t\t          검 강 화 게 임           \n";
    cout << "\t\t ==================================\n";
    cout << "\n\t\t    엔터를 누르면 시작합니다...";
    while (_getch() != 13);
    
    MainMenu();
    return 0;
}
