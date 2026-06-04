# SeaProject 구조 요약

> 한 줄 요약: **UE5 VR 템플릿 기반의 "바다 탐험 + 물고기 도감" VR 게임.**
> 플레이어가 VR로 바닷속을 돌아다니며 물고기를 스캔(손전등/소나)해서 도감(Encyclopedia)을 채우는 컨셉.
>
> ※ C++ 부분은 코드를 직접 읽고 정리한 "확정" 내용이고, 블루프린트(.uasset)는 바이너리라
>   내부 로직은 못 읽어서 **이름·폴더 구조·연결관계로 추정**한 내용임(추정은 "(추정)" 표기).

---

## 1. 핵심 컨셉 / 장르
- **VR(메타퀘스트류) 바닷속 탐험 게임**
- 핵심 루프: 헤엄쳐 다님 → 물고기 발견 → **스캔** → **도감(Encyclopedia) 등록** → 홀로그램으로 감상
- 보조 요소: 미니맵, 방향 화살표 인디케이터, 산소/부스터 게이지, 대시

---

## 2. C++ 코드 (Source/SeaProject) — 확정

### Player/XRPawn (AXRPawn) — VR 플레이어
- VR용 폰. 구성: `VRRoot` → `Camera`(HMD), `LeftHandController` / `RightHandController`(모션 컨트롤러), `FloatingPawnMovement`
- **Enhanced Input** 사용: `VRMappingContext`, `MoveAction`(조이스틱 Vector2D 이동), `GrabRightAction`
- 카메라가 바라보는 방향 기준으로 이동(`MoveSpeed = 300`)
- **[신규 추가]** 오른손에 `Flashlight`(SpotLight) + `ScanComponent`(손전등 스캔) 부착

### Player/MyPlayerPawn (AMyPlayerPawn) — 데스크톱(비VR) 플레이어
- 키보드/마우스용 폰. WASD 이동 + 마우스 시점 + **대시(Dash)**
- **부스터 게이지 시스템**(dash_boosterbar 작업물):
  - 평속 400 / 대시속도 1200
  - 부스터 최대 100, 대시 중 초당 50 소모, 평상시 초당 5 회복
  - 게이지 10% 이상일 때만 대시 시작 가능, 다 쓰면 자동 해제
- 입력은 (Enhanced Input이 아닌) 레거시 Axis/Action 바인딩 사용
- 용도: **PC에서 테스트하기 위한 폰**으로 보임(추정)

### Scan/ (신규 손전등 스캔 시스템) — 확정
- `ScannableInterface` : 물고기가 구현하는 인터페이스(도감 RowName 제공 + 스캔 콜백)
- `FlashlightScanComponent` : 손전등 방향으로 트레이스 → 물고기를 일정 시간 비추면 스캔 완료.
  스캔 시간은 `DT_Encyclopedia`에서 RowName 기준으로 읽음. **기존 Sonar 스캐너와 독립**.

> 관찰: VR 폰(Enhanced Input)과 데스크톱 폰(레거시 Input)이 공존 → 입력 매핑이 두 방식으로 나뉘어 있음.

---

## 3. 블루프린트 / 콘텐츠 (Content) — 폴더별

### CH/ — VR 코어 + 도감 (담당: chan hong 계열 추정)
| 에셋 | 역할(추정) |
|---|---|
| `BP_XRPawn` | VR 폰 블루프린트(AXRPawn 기반으로 추정) |
| `BP_VRGameMode` | VR 게임모드 (기본 폰 = BP_XRPawn) |
| `BP_PlayerPawn` / `BpPlayerPawn` | 플레이어 폰 변형들 |
| `BP_HologramDisplay` | 도감용 물고기 홀로그램 표시 장치 |
| `IMC_VR` | VR 입력 매핑 컨텍스트 |
| **Encyclopedia/DT_Encyclopedia** | **도감 데이터테이블** (물고기별 데이터 + 스캔시간 등) |
| **Encyclopedia/ST_EncyclopediaData** | 도감 행 구조체(데이터 형식) |
| **Encyclopedia/M_Hologram** | 홀로그램 머티리얼 |
| EnhancedInput/ IA_GrabLeft, IA_GrabRight, IA_Move, IMC_VR | VR 입력 액션/컨텍스트 |

### SY/ (175개) — 물고기 & 환경 (담당: SY 계열 추정)
- **SY/BP/** — 물고기 블루프린트:
  - `BP_FishBase` : 모든 물고기의 부모(공통 로직)
  - 개별 물고기 14종: `BP_BlobFish`, `BP_Bluetang`, `BP_Clione`, `BP_CrownFish`,
    `BP_JellyFish`, `BP_Lionfish`, `BP_Mandarine`, `BP_Octopus`, `BP_Seahorse`,
    `BP_Shark`, `BP_Turtle`, `BP_Whale`, `BP_Lure`
- **SY/Mesh/Fish/** — 각 물고기의 스켈레탈 메시 + 스켈레톤 + 피직스 에셋 + 애니메이션 + 머티리얼
  (Meshy AI / 직접 제작 메시들. 텍스처 다수)
- **SY/Map/** — `Boat`(보트), `Leviathan` 등 큰 오브젝트
- **SY/Mesh/** — 바닥 모래(Rippled Sand 8K), 환경 메시
- **SY/VFX/** — Niagara 파티클 시스템

### Sonar/ — 기존 소나 스캐너 (신규 손전등 스캔과 별개)
| 에셋 | 역할(추정) |
|---|---|
| `BP_Scanner` | 소나/스캐너 본체 |
| `BP_Item` | 스캔 대상 아이템 |
| `M_Scanner` / `M_HighlightItem` | 스캐너·하이라이트 머티리얼 |
| `PPM_Highlight` | 포스트프로세스 하이라이트(윤곽선 강조 추정) |

### UI / 인디케이터
- **UI/WBP_OxygenBar** — 산소(또는 부스터) 게이지 위젯 (dash_boosterbar 작업)
- **Arrow/** — `WBP_Arrow`, `WBP_Indicator` + 화살표 메시 : 목표 방향 표시 UI
- **MiniMap/** — `BP_MinimapCamera`(상공 카메라) + `RT_Minimap`(렌더타깃) + `WBP_Minimap` : 미니맵 시스템

### XRFramework/ (65개) — 언리얼 VR 템플릿 원본
- Epic의 표준 VR Template: `BP_XRPawn`, `BP_XRGameMode`, `BP_GrabComponent`(잡기),
  `BP_Grabbable_SmallCube`, `BP_Pistol`/`BP_Projectile`(총/투사체), `BP_TeleportVisualizer`(텔레포트),
  `BP_Menu`/`WBP_Menu`, 손 입력(IA_Hand_*), 햅틱, 텔레포트/메뉴 VFX
- → 프로젝트의 VR 상호작용 토대(잡기·텔레포트·메뉴 등)

### 기타 (대부분 템플릿/에셋팩 원본)
- **XRMannequins/** (53) — VR 손/몸 마네킹 메시
- **Weapons/** (27) — 무기 에셋(VR 템플릿 권총 등)
- **VRSpectator/** (11) — 관전자 화면
- **Fab/** (60) — Fab/Quixel 다운로드 에셋(이끼 바위 포토스캔 등)
- **LevelPrototyping/** (29) — 그레이박스(블록아웃) 메시·머티리얼

---

## 4. 맵(Level)
- `Content/Test.umap` — **실제 작업 중인 메인 맵**
- `Content/VRTemplate/VRTemplateMap.umap` — VR 템플릿 기본 맵(원본)

---

## 5. 한눈에 보는 기능별 담당 에셋
| 기능 | 관련 에셋 |
|---|---|
| VR 이동/조작 | XRPawn(C++), BP_XRPawn, IMC_VR, XRFramework |
| 데스크톱 테스트/대시 | MyPlayerPawn(C++), WBP_OxygenBar |
| 물고기 | SY/BP/BP_FishBase + 물고기 14종, SY/Mesh/Fish |
| 스캔(신규·손전등) | Scan/ (C++), XRPawn의 Flashlight |
| 스캔(기존·소나) | Sonar/BP_Scanner, BP_Item, PPM_Highlight |
| 도감 | CH/Encyclopedia/(DT/ST/M_Hologram), BP_HologramDisplay |
| 미니맵 | MiniMap/ |
| 방향 안내 | Arrow/ |

---

## 6. 참고/관찰
- VR(Enhanced Input)·데스크톱(레거시 Input) 두 입력 체계가 공존함
- 스캔 시스템이 2개(기존 Sonar BP / 신규 손전등 C++) — 의도적으로 분리됨
- 메시/물고기 에셋은 용량이 커서 별도 관리(학교 컴퓨터) 중인 항목 존재
- 블루프린트 내부 로직은 본 요약에서 직접 검증하지 못함(이름·구조 기반 추정)
