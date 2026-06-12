<div align="center">

# 🌊 SeaProject

### 심해를 탐험하고, 빛으로 생명을 기록하다 — VR 해양 탐사 게임

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-0E1128?style=for-the-badge&logo=unrealengine&logoColor=white)](https://www.unrealengine.com/)
[![Platform](https://img.shields.io/badge/Platform-Meta%20Quest%20%2F%20OpenXR-1C1E20?style=for-the-badge&logo=meta&logoColor=white)](https://www.meta.com/quest/)
[![Language](https://img.shields.io/badge/C%2B%2B%20%26%20Blueprint-blue?style=for-the-badge&logo=cplusplus&logoColor=white)](#-기술-스택)
![Status](https://img.shields.io/badge/status-In%20Development-yellow?style=for-the-badge)

</div>

---

## 🐟 소개

**SeaProject**는 플레이어가 직접 바닷속을 헤엄치며 심해를 탐사하는 **VR 해양 탐험 게임**입니다.
손전등으로 어둠을 밝히고, **소나 스캐너**로 물고기를 스캔해 **홀로그램 도감**에 하나씩 기록해 나갑니다.

> 고개를 숙이면 잠수하고, 들면 떠오릅니다. 몸으로 헤엄치는 진짜 바닷속을 경험하세요.

---

## ✨ 주요 기능

- 🏊 **몸으로 헤엄치는 이동** — 머리가 향하는 3D 방향으로 수영 (고개 각도로 잠수/상승)
- ⚡ **대시 부스트** — 순간 가속으로 빠르게 이동
- 🔦 **손전등** — 오른손에 들고 어둠 속을 비추기 (온/오프 토글)
- 📡 **소나 스캔** — 물고기를 스캔해 정보 수집 (`BP_Scanner`)
- 📖 **홀로그램 도감** — 수집한 생물을 홀로그램으로 열람, 페이지 넘김
- ✋ **물체 잡기(Grab)** — 양손 모션 컨트롤러로 물리 오브젝트 집기
- 🌊 **수심 연출** — 깊이에 따라 어두워지는 포스트프로세스 효과

---

## 🎮 조작법 (Meta Quest / Touch 컨트롤러)

| 컨트롤러 입력 | 동작 |
|---|---|
| 🕹️ **왼손 스틱** | 이동 (머리 방향으로 수영) |
| ✊ **왼손 그립** | 소나 스캔 |
| 🔘 **왼손 X** | 손전등 온 / 오프 |
| ✊ **오른손 그립** | 물체 잡기 |
| 🔘 **오른손 A** | 대시(부스트) |
| 🔘 **오른손 B** | 도감 열기 / 닫기 |
| 🕹️ **오른손 스틱 좌우** | 도감 페이지 넘김 |

---

## 🛠 기술 스택

| 구분 | 내용 |
|---|---|
| **엔진** | Unreal Engine 5.7 |
| **VR / 입력** | OpenXR · Enhanced Input (+ Hand/Eye Tracking) |
| **물 시뮬레이션** | UE Water Plugin |
| **언어** | C++ (게임플레이 코어) + Blueprint (연출·로직) |

### 핵심 C++ 클래스
- `AXRPawn` — VR 플레이어 폰 (이동 · 그랩 · 손전등 · 스캔/도감 입력)
- `UFlashlightScanComponent` — 손전등 기반 스캔 컴포넌트
- `IScannableInterface` — 스캔 가능한 대상(물고기 등) 인터페이스
- `AMyPlayerPawn` — 데스크톱/디버그용 폰

---

## 📁 프로젝트 구조

```
SeaProject/
├── Source/SeaProject/
│   ├── Player/        # XRPawn, MyPlayerPawn (플레이어 폰)
│   └── Scan/          # FlashlightScanComponent, ScannableInterface
├── Content/
│   ├── CH/            # 플레이어 폰 BP · Enhanced Input
│   ├── Sonar/         # BP_Scanner, 스캔 머티리얼
│   ├── SY/BP/         # BP_FishBase (물고기 베이스)
│   ├── Arrow/ · UI/   # 인디케이터 · 위젯
│   └── ...            # 환경 에셋 (DeepWaterStation 등)
└── SeaProject.uproject
```

---

## 🚀 빌드 & 실행

```text
1. SeaProject.uproject 우클릭 → "Generate Visual Studio project files"
2. SeaProject.sln 열어서 빌드 (Development Editor / Win64)
3. .uproject 실행 → VR 헤드셋 연결 후 Play
```

> 💡 C++ 프로젝트라 **Visual Studio**(또는 Rider)가 필요합니다. 처음 열 때 "리빌드?" 창이 뜨면 **예**를 누르세요.

---

<div align="center">

🐠 *Dive deep. Light the dark. Catalog life.* 🐠

</div>
