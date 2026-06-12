const pptxgen = require("pptxgenjs");
const p = new pptxgen();
p.layout = "LAYOUT_WIDE"; const W = 13.33, H = 7.5;
p.author = "DEEP DIVE VR Team"; p.title = "DEEP DIVE VR 기말 발표";

const BG0 = "01101E", BG1 = "082039", BG2 = "08365C", CARD = "0E3356", CARD2 = "0B2A47", BORDER = "1E5C86", CODEBG = "05101C";
const CYAN = "00D3FF", CYAN2 = "53DCFF", MINT = "00E2BE", GOLD = "FFCC4D", PINK = "F472B6", PURPLE = "9A6CFF";
const WHITE = "F7FCFF", TXT = "C2ECFF", MUTE = "9FC4DC", DIM = "6F93AC", CMT = "6FB39A", CODE = "D6EAF5";
const NK = "Noto Sans KR", BLACK = "Arial Black", MONO = "Consolas";
const sh = () => ({ type: "outer", color: "000814", blur: 10, offset: 3, angle: 90, opacity: 0.35 });
const AC = [CYAN, MINT, GOLD, PINK, PURPLE, CYAN2];

function bg(s) {
  s.background = { color: BG0 };
  s.addShape(p.shapes.OVAL, { x: -2.5, y: -4.5, w: 12, h: 9, fill: { color: BG2, transparency: 64 }, line: { type: "none" } });
  s.addShape(p.shapes.OVAL, { x: 8.0, y: 4.0, w: 9.5, h: 8.5, fill: { color: "0A3A5E", transparency: 76 }, line: { type: "none" } });
}
function sec(s, num, kr, en) {
  s.addText(num, { x: 0.72, y: 0.46, w: 1.15, h: 0.82, fontFace: BLACK, fontSize: 28, color: CYAN, valign: "middle", margin: 0 });
  s.addText(kr, { x: 1.9, y: 0.46, w: 10.7, h: 0.62, fontFace: NK, fontSize: 25, bold: true, color: WHITE, valign: "middle", margin: 0 });
  s.addText(en.toUpperCase(), { x: 1.93, y: 1.12, w: 10.6, h: 0.32, fontFace: NK, fontSize: 11, bold: true, color: MINT, charSpacing: 3, margin: 0 });
}
function card(s, x, y, w, h, fill) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.09, fill: { color: fill || CARD }, line: { color: BORDER, width: 1 }, shadow: sh() });
}
function foot(s, n) {
  s.addText("DEEP DIVE VR", { x: 0.72, y: H - 0.5, w: 4, h: 0.3, fontFace: NK, fontSize: 9, bold: true, color: DIM, charSpacing: 2, margin: 0 });
  s.addText(String(n).padStart(2, "0"), { x: W - 1.0, y: H - 0.5, w: 0.6, h: 0.3, fontFace: BLACK, fontSize: 10, color: DIM, align: "right", margin: 0 });
}
function dot(s, x, y, c, d = 0.34) { s.addShape(p.shapes.OVAL, { x, y, w: d, h: d, fill: { color: c }, line: { type: "none" } }); }
// code panel: lines = [{t, c}]
function codePanel(s, x, y, w, h, lines) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.07, fill: { color: CODEBG }, line: { color: BORDER, width: 1 }, shadow: sh() });
  ["FF5F56", "FFBD2E", "27C93F"].forEach((c, i) => s.addShape(p.shapes.OVAL, { x: x + 0.22 + i * 0.24, y: y + 0.2, w: 0.13, h: 0.13, fill: { color: c }, line: { type: "none" } }));
  s.addText("C++", { x: x + w - 1.1, y: y + 0.12, w: 0.9, h: 0.3, fontFace: MONO, fontSize: 10, color: DIM, align: "right", margin: 0 });
  const runs = lines.map(ln => ({ text: ln.t === "" ? " " : ln.t, options: { breakLine: true, color: ln.c || CODE } }));
  s.addText(runs, { x: x + 0.32, y: y + 0.55, w: w - 0.55, h: h - 0.75, fontFace: MONO, fontSize: lines.length > 12 ? 11 : 12, lineSpacingMultiple: 1.18, valign: "top", margin: 0 });
}
// feature + code slide
function codeSlide(s, num, name, en, bullets, lines) {
  bg(s); sec(s, num, name, en); foot(s, parseInt(num, 10) + 1);
  bullets.forEach((b, i) => {
    const y = 2.0 + i * 1.45; dot(s, 0.8, y + 0.05, AC[i % AC.length], 0.36);
    s.addText(b[0], { x: 1.35, y: y, w: 3.9, h: 0.45, fontFace: NK, fontSize: 16, bold: true, color: WHITE, margin: 0 });
    s.addText(b[1], { x: 1.35, y: y + 0.46, w: 4.0, h: 0.85, fontFace: NK, fontSize: 12.5, color: MUTE, lineSpacingMultiple: 1.2, margin: 0 });
  });
  codePanel(s, 5.55, 1.9, 7.05, 4.85, lines);
}

// ════════ 1. TITLE ════════
let s = p.addSlide(); bg(s);
s.addText("VR DEEP SEA EXPLORATION", { x: 0.9, y: 1.55, w: 11, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: CYAN, charSpacing: 5, margin: 0 });
s.addText("DEEP DIVE VR", { x: 0.84, y: 2.0, w: 12, h: 1.7, fontFace: BLACK, fontSize: 70, color: WHITE, margin: 0 });
s.addText("VR 심해 탐사 시뮬레이션", { x: 0.9, y: 3.75, w: 11, h: 0.6, fontFace: NK, fontSize: 24, bold: true, color: CYAN2, margin: 0 });
s.addText("게임 소개 · 게임 흐름 · 주요 기능과 코드", { x: 0.92, y: 4.4, w: 11.5, h: 0.45, fontFace: NK, fontSize: 15, color: TXT, margin: 0 });
["2026 XR UI/UX 프로그래밍", "기말 발표", "TEAM 박찬홍 (팀장·기획·프로그래밍)"].forEach((c, i) => {
  const cx = 0.9 + i * 3.7;
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: cx, y: 5.35, w: 3.5, h: 0.58, rectRadius: 0.29, fill: { color: CARD2 }, line: { color: BORDER, width: 1 } });
  s.addText(c, { x: cx, y: 5.35, w: 3.5, h: 0.58, fontFace: NK, fontSize: 11.5, bold: true, color: i === 1 ? GOLD : CYAN2, align: "center", valign: "middle", margin: 0 });
});
s.addText("발표 8분  +  시연 영상 2분", { x: 0.92, y: 6.45, w: 8, h: 0.4, fontFace: NK, fontSize: 12, color: DIM, margin: 0 });

// ════════ 2. 게임 설명 ════════
s = p.addSlide(); bg(s); sec(s, "01", "게임 설명", "What is DEEP DIVE VR"); foot(s, 2);
card(s, 0.72, 1.85, 11.9, 1.45, BG1);
s.addText([
  { text: "DEEP DIVE VR", options: { color: CYAN, bold: true } },
  { text: " 는 플레이어가 VR로 심해를 탐사하며, ", options: { color: TXT } },
  { text: "소나로 생물을 발견하고 손전등으로 스캔", options: { color: CYAN2, bold: true } },
  { text: "해 ", options: { color: TXT } },
  { text: "홀로그램 도감", options: { color: GOLD, bold: true } },
  { text: "을 채워가는 1인칭 심해 탐사 시뮬레이션입니다.", options: { color: TXT } },
], { x: 1.1, y: 1.85, w: 11.1, h: 1.45, fontFace: NK, fontSize: 18, valign: "middle", lineSpacingMultiple: 1.3, margin: 0 });
const ov = [["장르 · 플랫폼", "VR 탐험 / 수집 시뮬레이션\nOpenXR 기반 (HMD + 모션컨트롤러)", CYAN],
["핵심 경험", "전투가 아닌 '관찰과 수집'\n빛이 닿지 않는 심해의 공포와 경이", MINT],
["차별점", "능동적 손전등 스캔 + 반응형 생물\n다이제틱 UI로 깊은 몰입", GOLD]];
ov.forEach((c, i) => {
  const x = 0.72 + i * 4.06; card(s, x, 3.6, 3.82, 3.2);
  s.addShape(p.shapes.RECTANGLE, { x, y: 3.6, w: 0.1, h: 3.2, fill: { color: c[2] }, line: { type: "none" } });
  s.addText(c[0], { x: x + 0.3, y: 3.85, w: 3.3, h: 0.5, fontFace: NK, fontSize: 16, bold: true, color: c[2], margin: 0 });
  s.addText(c[1], { x: x + 0.3, y: 4.45, w: 3.3, h: 2.0, fontFace: NK, fontSize: 13.5, color: TXT, lineSpacingMultiple: 1.3, margin: 0 });
});

// ════════ 3. 게임 흐름 ════════
s = p.addSlide(); bg(s); sec(s, "02", "게임 흐름", "Gameplay Flow"); foot(s, 3);
s.addText("한 번의 플레이는 이 4단계를 돌며 진행됩니다. 도감을 채우려 더 깊이, 더 많이 탐사하게 됩니다.", { x: 0.72, y: 1.62, w: 12, h: 0.45, fontFace: NK, fontSize: 15, color: MUTE, margin: 0 });
const loop = [["🌊", "탐사", "수중 추진기로\n심해를 유영", CYAN], ["📡", "발견", "소나로 주변\n생물 위치 포착", MINT], ["🔦", "스캔", "손전등을 겨눠\n진행도를 채움", GOLD], ["📖", "도감", "수집 영구 등록\n다시 와도 유지", CYAN2]];
const cw = 2.8, gap = 0.43, sx = (W - (cw * 4 + gap * 3)) / 2, cy = 2.55;
loop.forEach((l, i) => {
  const x = sx + i * (cw + gap); card(s, x, cy, cw, 3.1);
  s.addShape(p.shapes.OVAL, { x: x + cw / 2 - 0.55, y: cy + 0.35, w: 1.1, h: 1.1, fill: { color: CARD2 }, line: { color: l[3], width: 1.5 } });
  s.addText(l[0], { x: x + cw / 2 - 0.55, y: cy + 0.35, w: 1.1, h: 1.1, fontFace: NK, fontSize: 30, align: "center", valign: "middle", margin: 0 });
  s.addText(String(i + 1).padStart(2, "0"), { x: x, y: cy + 1.55, w: cw, h: 0.4, fontFace: BLACK, fontSize: 14, color: l[3], align: "center", margin: 0 });
  s.addText(l[1], { x: x, y: cy + 1.95, w: cw, h: 0.45, fontFace: NK, fontSize: 18, bold: true, color: WHITE, align: "center", margin: 0 });
  s.addText(l[2], { x: x + 0.2, y: cy + 2.42, w: cw - 0.4, h: 0.6, fontFace: NK, fontSize: 12.5, color: MUTE, align: "center", lineSpacingMultiple: 1.1, margin: 0 });
  if (i < 3) s.addText("›", { x: x + cw - 0.04, y: cy + 0.7, w: gap + 0.08, h: 1.0, fontFace: NK, fontSize: 28, bold: true, color: CYAN, align: "center", valign: "middle", margin: 0 });
});
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: sx + cw * 1.5, y: 6.05, w: cw, h: 0.5, rectRadius: 0.25, fill: { color: CARD2 }, line: { color: BORDER, width: 1 } });
s.addText("↻  반복하며 도감 완성", { x: sx + cw * 1.5, y: 6.05, w: cw, h: 0.5, fontFace: NK, fontSize: 12, bold: true, color: CYAN2, align: "center", valign: "middle", margin: 0 });

// ════════ 4. 코드① 손전등 스캔(락온) ════════
s = p.addSlide();
codeSlide(s, "03", "주요 기능 ① 손전등 스캔 (락온)", "Lock-on Scan",
  [["락온 방식", "한 번 포착하면 도망쳐 시야를 벗어나도 스캔 유지"],
  ["진행도 누적", "매 틱 시간을 더해 100%에서 완료 판정"],
  ["생물별 시간", "데이터값으로 종마다 스캔 난이도가 다름"]],
  [
    { t: "void UpdateFlashlightScan(float Dt){", c: CYAN2 },
    { t: "  if(!bScanActive){ Reset(); return; }", c: CODE },
    { t: "  // 락온: 콘 안의 물고기를 한 번만 포착", c: CMT },
    { t: "  if(!CurrentScanTarget){", c: CODE },
    { t: "    AActor* F = GetFlashlightScanTarget();", c: CODE },
    { t: "    if(!F) return;            // 아직 못 비춤", c: CODE },
    { t: "    CurrentScanTarget   = F;", c: GOLD },
    { t: "    CurrentScanDuration = ReadScanTime(F);", c: GOLD },
    { t: "  }", c: CODE },
    { t: "  SetFishFleeing(CurrentScanTarget, true);", c: CODE },
    { t: "  ScanProgress += Dt;         // 진행도 누적", c: CODE },
    { t: "  if(ScanProgress >= CurrentScanDuration)", c: CYAN },
    { t: "    OnFishScanned(CurrentScanTarget); // 도감", c: CYAN },
    { t: "}", c: CYAN2 },
  ]);

// ════════ 5. 코드② 콘 판정 ════════
s = p.addSlide();
codeSlide(s, "04", "주요 기능 ② 스캔 타겟 판정", "Angular-Radius Cone",
  [["콜리전 비의존", "Fish 태그 + 각도·거리로 안정적 판정"],
  ["벡터 기하", "내적과 acos 로 손전등과의 사잇각 계산"],
  ["각반경 보정", "물고기 크기·거리로 허용각을 자동 조절"]],
  [
    { t: "AActor* GetFlashlightScanTarget(){", c: CYAN2 },
    { t: "  for(AActor* F : FishActors){", c: CODE },
    { t: "    FVector to = F->Center - Start;", c: CODE },
    { t: "    float dist = to.Size();", c: CODE },
    { t: "    if(dist > ScanRange) continue; // 사거리", c: CMT },
    { t: "    // 손전등 정면과의 사잇각", c: CMT },
    { t: "    float ang = acos(Forward | to.Normal());", c: GOLD },
    { t: "    // 물고기 크기만큼 허용각 보정", c: CMT },
    { t: "    float r = asin(Radius / dist);", c: GOLD },
    { t: "    if(ang > ConeHalfAngle + r) continue;", c: CYAN },
    { t: "    // -> 콘 안. 가장 가까운 것 선택", c: CMT },
    { t: "  }", c: CODE },
    { t: "}", c: CYAN2 },
  ]);

// ════════ 6. 코드③ 데이터 기반 도감 ════════
s = p.addSlide();
codeSlide(s, "05", "주요 기능 ③ 데이터 기반 도감", "Data-Driven Codex",
  [["표 한 줄 = 새 생물", "DataTable에 행만 추가하면 즉시 등장·등록"],
  ["영구 저장", "Subsystem에 수집 기록 → 닫아도 유지"],
  ["리플렉션 읽기", "필드 이름으로 표 값을 유연하게 조회"]],
  [
    { t: "// 표에서 생물별 ScanTime 읽기", c: CMT },
    { t: "bool GetRowScanTime(Table,Row,float& Out){", c: CYAN2 },
    { t: "  auto* Data = FindResolvedRow(Table,Row);", c: CODE },
    { t: "  FString s = ReadField(Data, \"ScanTime\");", c: GOLD },
    { t: "  float v = FCString::Atof(*s);", c: CODE },
    { t: "  if(v <= 0) return false;  // 없으면 기본값", c: CODE },
    { t: "  Out = v; return true;", c: CODE },
    { t: "}", c: CYAN2 },
    { t: "", c: CODE },
    { t: "// 수집은 영구 저장 (도감 한 바퀴 돌아도 유지)", c: CMT },
    { t: "Subsystem->MarkCollected(Row);", c: CYAN },
  ]);

// ════════ 7. 코드④ 퍼지 이름 매칭 ════════
s = p.addSlide();
codeSlide(s, "06", "주요 기능 ④ 퍼지 이름 매칭", "Fuzzy Matching · Edit Distance",
  [["오타 보정", "대소문자·접두어·1글자 오타까지 매칭"],
  ["4단계 폴백", "완전 -> 정규화 -> 부분 -> 편집거리 순"],
  ["2행 롤링 DP", "레벤슈타인 거리, 메모리 O(n)"]],
  [
    { t: "// 4단계: 완전->정규화->부분->편집거리<=1", c: CMT },
    { t: "int EditDistance(A, B){   // 레벤슈타인", c: CYAN2 },
    { t: "  for(int i=1;i<=lenA;i++)", c: CODE },
    { t: "   for(int j=1;j<=lenB;j++)", c: CODE },
    { t: "    Cur[j] = Min3(Prev[j]+1, Cur[j-1]+1,", c: GOLD },
    { t: "           Prev[j-1] + (A[i]==B[j]?0:1));", c: GOLD },
    { t: "  return Prev[lenB];     // 2행 롤링 DP", c: CODE },
    { t: "}", c: CYAN2 },
    { t: "", c: CODE },
    { t: "// \"madarine\" <-> \"mandarine\" => 1 => 매칭 OK", c: CMT },
  ]);

// ════════ 8. 코드⑤ 물고기 AI ════════
s = p.addSlide();
codeSlide(s, "07", "주요 기능 ⑤ 물고기 AI", "Fish AI · Steering",
  [["도망 스티어링", "스캔당하면 위협 반대 벡터로 방향 전환"],
  ["예측형 회피", "앞을 미리 스윕해 충돌 전에 틀기"],
  ["부드러운 회전", "등속 보간으로 자연스러운 움직임"]],
  [
    { t: "void TickComponent(float Dt){", c: CYAN2 },
    { t: "  if(bFleeing){            // 위협 반대로", c: CMT },
    { t: "    FVector away = Loc - ThreatLoc;", c: GOLD },
    { t: "    away.Z = 0;", c: CODE },
    { t: "    TargetRot.Yaw = away.Rotation().Yaw;", c: GOLD },
    { t: "  } else { Wander(); }     // 평소 배회", c: CODE },
    { t: "  // 앞을 미리 스윕 -> 충돌 전 회피", c: CMT },
    { t: "  if(SweepAhead(Hit))", c: CODE },
    { t: "    TargetRot.Yaw = Hit.Normal.Rotation().Yaw;", c: CYAN },
    { t: "  Rot = RInterpConstantTo(Rot,TargetRot,Dt,Turn);", c: CODE },
    { t: "  Move(Fwd*(bFleeing?FleeSpeed:SwimSpeed)*Dt);", c: CODE },
    { t: "}", c: CYAN2 },
  ]);

// ════════ 9. 코드⑥ 다이제틱 UI & 사운드 ════════
s = p.addSlide();
codeSlide(s, "08", "주요 기능 ⑥ 다이제틱 UI & 사운드", "Diegetic UI · Audio",
  [["손목 수심계", "UI를 화면이 아닌 손목(월드)에 — 다이제틱"],
  ["BGM 덕킹", "스캔·소나 중 음악 정지 -> 끝나면 이어 재생"],
  ["효과음 분리", "스캔/완료/소나 가동·감지음 구분"]],
  [
    { t: "// 손목 수심계 (유닛 -> 미터 환산)", c: CMT },
    { t: "float GetPlayerDepthMeters(){", c: CYAN2 },
    { t: "  float d = WaterSurfaceZ - GetLocation().Z;", c: GOLD },
    { t: "  return d>0 ? d / DepthUnitsPerMeter : 0;", c: GOLD },
    { t: "}", c: CYAN2 },
    { t: "", c: CODE },
    { t: "// 스캔·소나 중 배경음 자동 정지/재개", c: CMT },
    { t: "void SetBackgroundMusicPaused(bool b){", c: CYAN2 },
    { t: "  if(CachedBGM) CachedBGM->SetPaused(b);", c: CYAN },
    { t: "}", c: CYAN2 },
  ]);

// ════════ 10. 기술 스택 ════════
s = p.addSlide(); bg(s); sec(s, "09", "기술 스택 & 구조", "Tech Stack"); foot(s, 10);
const tech = [["ENGINE", "Unreal Engine 5", ["Lumen 실시간 GI", "Nanite 메시 최적화", "Water 수중 환경"], CYAN],
["CODE", "C++ / Blueprint", ["핵심 로직 C++", "연출·이벤트 BP", "모듈형 설계"], MINT],
["DATA / XR", "DataTable · OpenXR", ["도감 데이터 구동", "Subsystem 영구저장", "OpenXR VR 입력"], GOLD]];
tech.forEach((t, i) => {
  const x = 0.72 + i * 4.06; card(s, x, 1.95, 3.82, 4.55);
  s.addShape(p.shapes.RECTANGLE, { x, y: 1.95, w: 0.1, h: 4.55, fill: { color: t[3] }, line: { type: "none" } });
  s.addText(t[0], { x: x + 0.32, y: 2.2, w: 3.3, h: 0.35, fontFace: NK, fontSize: 12, bold: true, color: t[3], charSpacing: 2, margin: 0 });
  s.addText(t[1], { x: x + 0.32, y: 2.58, w: 3.3, h: 0.6, fontFace: NK, fontSize: 19, bold: true, color: WHITE, margin: 0 });
  s.addText(t[2].map(it => ({ text: it, options: { bullet: { code: "2022", indent: 14 }, breakLine: true, color: TXT } })), { x: x + 0.45, y: 3.35, w: 3.15, h: 2.9, fontFace: NK, fontSize: 13.5, lineSpacingMultiple: 1.35, valign: "top", margin: 0 });
});

// ════════ 11. 시연 영상 ════════
s = p.addSlide(); bg(s);
s.addShape(p.shapes.OVAL, { x: W / 2 - 0.9, y: 2.0, w: 1.8, h: 1.8, fill: { color: CYAN }, line: { type: "none" }, shadow: sh() });
s.addText("▶", { x: W / 2 - 0.72, y: 2.0, w: 1.8, h: 1.8, fontFace: NK, fontSize: 42, color: BG0, align: "center", valign: "middle", margin: 0 });
s.addText("DEMO", { x: 0, y: 3.95, w: W, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: MINT, align: "center", charSpacing: 5, margin: 0 });
s.addText("시연 영상", { x: 0, y: 4.35, w: W, h: 0.8, fontFace: BLACK, fontSize: 44, color: WHITE, align: "center", margin: 0 });
s.addText("약 2분  ·  탐사 → 소나 발견 → 스캔 → 도감 등록의 실제 플레이", { x: 0, y: 5.3, w: W, h: 0.5, fontFace: NK, fontSize: 17, color: TXT, align: "center", margin: 0 });

// ════════ 12. 마무리 ════════
s = p.addSlide(); bg(s);
s.addText("THANK YOU", { x: 0.9, y: 1.5, w: 11, h: 0.5, fontFace: NK, fontSize: 14, bold: true, color: CYAN, charSpacing: 5, margin: 0 });
s.addText("심해의 발견을 도감에 담다", { x: 0.84, y: 1.98, w: 12, h: 1.0, fontFace: BLACK, fontSize: 38, color: WHITE, margin: 0 });
s.addText("핵심 요약", { x: 0.95, y: 3.4, w: 5.5, h: 0.4, fontFace: NK, fontSize: 16, bold: true, color: GOLD, margin: 0 });
s.addText([{ text: "VR 심해 탐사 + 소나·스캔으로 도감 수집", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "락온 스캔 · 데이터 기반 도감 · 영구 저장", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "퍼지 매칭·콘 판정·생물 AI 등 핵심 코드 구현", options: { bullet: { code: "2022" } } }], { x: 0.95, y: 3.85, w: 5.7, h: 2.2, fontFace: NK, fontSize: 14, color: TXT, lineSpacingMultiple: 1.4, margin: 0 });
s.addText("향후 계획", { x: 7.0, y: 3.4, w: 5.5, h: 0.4, fontFace: NK, fontSize: 16, bold: true, color: GOLD, margin: 0 });
s.addText([{ text: "해양 생물 종 추가 및 희귀종 도입", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "미션/퀘스트 · 사진 모드 등 콘텐츠 확장", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "사운드·비주얼 폴리싱으로 몰입 강화", options: { bullet: { code: "2022" } } }], { x: 7.0, y: 3.85, w: 5.6, h: 2.2, fontFace: NK, fontSize: 14, color: TXT, lineSpacingMultiple: 1.4, margin: 0 });
s.addText("DEEP DIVE VR  ·  Unreal Engine 5  ·  OpenXR", { x: 0, y: 6.7, w: W, h: 0.4, fontFace: NK, fontSize: 11, color: DIM, align: "center", margin: 0 });

p.writeFile({ fileName: "C:/Users/pch31/Documents/GitHub/Seaproject/발표자료/DeepDiveVR_기말발표_코드중심.pptx" }).then(f => console.log("SAVED:", f));
