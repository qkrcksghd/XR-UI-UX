const pptxgen = require("pptxgenjs");
const p = new pptxgen();
p.layout = "LAYOUT_WIDE"; // 13.33 x 7.5
p.author = "SeaProject Team";
p.title = "SeaProject 발표자료";

const W = 13.33, H = 7.5;
// ── Palette (Deep Ocean) ─────────────────────────────
const NAVY = "06223B", DEEP = "0B3A5B", BLUE = "0E4D6E", TEAL = "12869A",
      CYAN = "6FC9DC", CORAL = "FF7A59", GOLD = "FFC857", WHITE = "FFFFFF",
      PANEL = "EAF3F7", PANEL2 = "F4F9FB", INK = "13303F", MUTE = "5E7B8A",
      EDGE = "D6E5EC";
const F = "Malgun Gothic";

const shadow = () => ({ type: "outer", color: "0B2A3D", blur: 9, offset: 3, angle: 90, opacity: 0.18 });

// ── helpers ─────────────────────────────────────────
function darkBG(s) {
  s.background = { color: NAVY };
  // surface light rays (subtle)
  s.addShape(p.shapes.RECTANGLE, { x: 0, y: 0, w: W, h: 1.7, fill: { color: DEEP, transparency: 35 }, line: { type: "none" } });
  s.addShape(p.shapes.OVAL, { x: -2, y: -3.2, w: 9, h: 6, fill: { color: BLUE, transparency: 70 }, line: { type: "none" } });
  s.addShape(p.shapes.OVAL, { x: 8.5, y: 4.2, w: 8, h: 7, fill: { color: TEAL, transparency: 82 }, line: { type: "none" } });
}
function lightBG(s) {
  s.background = { color: WHITE };
  // left vertical deep band motif
  s.addShape(p.shapes.RECTANGLE, { x: 0, y: 0, w: 0.18, h: H, fill: { color: TEAL }, line: { type: "none" } });
}
function kicker(s, txt) {
  s.addText(txt.toUpperCase(), { x: 0.75, y: 0.5, w: 8, h: 0.34, fontFace: F, fontSize: 12, bold: true, color: TEAL, charSpacing: 3, margin: 0 });
}
function heading(s, txt) {
  s.addText(txt, { x: 0.72, y: 0.82, w: 11.8, h: 0.85, fontFace: F, fontSize: 32, bold: true, color: INK, margin: 0 });
}
function pageNo(s, n) {
  s.addText(String(n).padStart(2, "0"), { x: W - 1.0, y: H - 0.55, w: 0.6, h: 0.3, fontFace: F, fontSize: 11, color: MUTE, align: "right", margin: 0 });
  s.addText("SeaProject", { x: 0.72, y: H - 0.55, w: 3, h: 0.3, fontFace: F, fontSize: 10, color: MUTE, margin: 0 });
}
function numCircle(s, x, y, n, d = 0.62, col = TEAL) {
  s.addShape(p.shapes.OVAL, { x, y, w: d, h: d, fill: { color: col }, line: { type: "none" }, shadow: shadow() });
  s.addText(String(n), { x, y, w: d, h: d, fontFace: F, fontSize: 20, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
}
function card(s, x, y, w, h, fill = PANEL2) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.1, fill: { color: fill }, line: { color: EDGE, width: 1 }, shadow: shadow() });
}

// ════════════════════════════════════════════════════
// 1. TITLE
// ════════════════════════════════════════════════════
let s = p.addSlide(); darkBG(s);
s.addText("UNDERWATER  VR  EXPLORATION", { x: 1.0, y: 1.85, w: 11, h: 0.4, fontFace: F, fontSize: 14, bold: true, color: CYAN, charSpacing: 4, margin: 0 });
s.addText("Sea Project", { x: 0.92, y: 2.25, w: 11.5, h: 1.5, fontFace: F, fontSize: 66, bold: true, color: WHITE, margin: 0 });
s.addText("바닷속을 헤엄치며 해양 생물을 발견하고, 나만의 도감을 완성하는 VR 탐험 게임", { x: 1.0, y: 3.75, w: 11.3, h: 0.6, fontFace: F, fontSize: 19, color: "CFE6EE", margin: 0 });
// accent chips
const chips = ["Unreal Engine 5.7", "OpenXR (VR)", "C++ / Blueprint"];
chips.forEach((c, i) => {
  const cx = 1.0 + i * 2.95;
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: cx, y: 5.05, w: 2.75, h: 0.6, rectRadius: 0.3, fill: { color: "0E3D5C" }, line: { color: TEAL, width: 1 } });
  s.addText(c, { x: cx, y: 5.05, w: 2.75, h: 0.6, fontFace: F, fontSize: 13, bold: true, color: CYAN, align: "center", valign: "middle", margin: 0 });
});
s.addText("8분 발표  +  2분 시연 영상", { x: 1.0, y: 6.35, w: 8, h: 0.4, fontFace: F, fontSize: 13, color: MUTE, margin: 0 });

// ════════════════════════════════════════════════════
// 2. 개요
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Overview"); heading(s, "한눈에 보는 SeaProject"); pageNo(s, 2);
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 0.72, y: 1.85, w: 11.9, h: 1.35, rectRadius: 0.1, fill: { color: DEEP }, line: { type: "none" }, shadow: shadow() });
s.addText([
  { text: "플레이어는 VR로 바닷속을 자유롭게 헤엄치며, ", options: { color: "CFE6EE" } },
  { text: "손전등 스캔과 소나", options: { color: GOLD, bold: true } },
  { text: "로 해양 생물을 발견·수집해 ", options: { color: "CFE6EE" } },
  { text: "나만의 도감", options: { color: GOLD, bold: true } },
  { text: "을 완성하는 힐링 탐험 게임입니다.", options: { color: "CFE6EE" } },
], { x: 1.1, y: 1.85, w: 11.1, h: 1.35, fontFace: F, fontSize: 18, valign: "middle", lineSpacingMultiple: 1.25, margin: 0 });

const pillars = [
  ["몰입형 VR 탐험", "헤드셋과 모션 컨트롤러로 진짜 바닷속을 헤엄치는 듯한 1인칭 경험"],
  ["발견과 수집의 재미", "전투가 아닌 '관찰·수집' 중심. 스캔으로 도감을 채워가는 성취감"],
  ["살아있는 수중 생태계", "물고기가 스스로 헤엄치고, 다가가면 반응하며 도망치는 생태"],
];
pillars.forEach((c, i) => {
  const x = 0.72 + i * 4.06;
  card(s, x, 3.55, 3.82, 3.2);
  numCircle(s, x + 0.3, 3.85, i + 1, 0.62, [TEAL, BLUE, CORAL][i]);
  s.addText(c[0], { x: x + 0.3, y: 4.65, w: 3.25, h: 0.5, fontFace: F, fontSize: 18, bold: true, color: INK, margin: 0 });
  s.addText(c[1], { x: x + 0.3, y: 5.15, w: 3.25, h: 1.5, fontFace: F, fontSize: 14, color: MUTE, lineSpacingMultiple: 1.2, margin: 0 });
});

// ════════════════════════════════════════════════════
// 3. 기획 의도 & 차별점
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Concept"); heading(s, "기획 의도 & 차별점"); pageNo(s, 3);
// left intent
card(s, 0.72, 1.9, 5.6, 4.85, DEEP);
s.addText("기획 의도", { x: 1.05, y: 2.2, w: 5, h: 0.5, fontFace: F, fontSize: 20, bold: true, color: GOLD, margin: 0 });
s.addText([
  { text: "경쟁과 전투에 지친 사람들에게,", options: { breakLine: true, color: WHITE, bold: true } },
  { text: "느긋하게 바닷속을 거닐며 생명을 관찰하는", options: { breakLine: true, color: "CFE6EE" } },
  { text: "'힐링과 몰입'의 경험을 주고 싶었습니다.", options: { breakLine: true, color: "CFE6EE" } },
  { text: "", options: { breakLine: true, fontSize: 8 } },
  { text: "VR의 강력한 몰입감 + 도감 수집의 동기부여를", options: { breakLine: true, color: "CFE6EE" } },
  { text: "결합해, 누구나 부담 없이 즐기는 탐험을 목표로 했습니다.", options: { color: "CFE6EE" } },
], { x: 1.05, y: 2.85, w: 5.0, h: 3.6, fontFace: F, fontSize: 15, lineSpacingMultiple: 1.3, valign: "top", margin: 0 });
// right differentiators
const diffs = [
  ["능동적 스캔 상호작용", "단순 클릭이 아닌, 손전등을 직접 겨누고 유지하는 손맛"],
  ["데이터 기반 도감", "표(DataTable)에 한 줄만 추가하면 새 생물이 바로 등장·등록"],
  ["반응하는 생태계", "스캔하면 물고기가 반대로 도망 → 살아있다는 긴장감"],
];
diffs.forEach((d, i) => {
  const y = 1.9 + i * 1.66;
  card(s, 6.62, y, 6.0, 1.48);
  numCircle(s, 6.92, y + 0.42, i + 1, 0.62, CORAL);
  s.addText(d[0], { x: 7.75, y: y + 0.22, w: 4.7, h: 0.5, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 7.75, y: y + 0.72, w: 4.7, h: 0.6, fontFace: F, fontSize: 13.5, color: MUTE, margin: 0 });
});

// ════════════════════════════════════════════════════
// 4. 핵심 플레이 루프
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Core Loop"); heading(s, "핵심 플레이 루프"); pageNo(s, 4);
s.addText("탐험하고 → 발견하고 → 스캔하고 → 도감을 채운다. 이 사이클의 반복이 게임의 핵심 재미입니다.", { x: 0.72, y: 1.7, w: 12, h: 0.5, fontFace: F, fontSize: 16, color: MUTE, margin: 0 });
const loop = [
  ["탐험", "VR로 바닷속을\n자유롭게 유영", TEAL],
  ["발견", "소나·시야로\n생물의 위치 포착", BLUE],
  ["스캔", "손전등을 겨눠\n진행도를 채움", CORAL],
  ["수집", "도감에 영구 등록\n(다시 와도 유지)", "1C9B7A"],
];
const cw = 2.78, gap = 0.42, startX = (W - (cw * 4 + gap * 3)) / 2, cy = 2.85;
loop.forEach((l, i) => {
  const x = startX + i * (cw + gap);
  card(s, x, cy, cw, 2.95);
  numCircle(s, x + cw / 2 - 0.4, cy + 0.35, i + 1, 0.8, l[2]);
  s.addText(l[0], { x: x, y: cy + 1.3, w: cw, h: 0.5, fontFace: F, fontSize: 21, bold: true, color: INK, align: "center", margin: 0 });
  s.addText(l[1], { x: x + 0.2, y: cy + 1.85, w: cw - 0.4, h: 1.0, fontFace: F, fontSize: 14, color: MUTE, align: "center", lineSpacingMultiple: 1.15, margin: 0 });
  if (i < 3) s.addText("›", { x: x + cw - 0.05, y: cy + 0.9, w: gap + 0.1, h: 0.8, fontFace: F, fontSize: 30, bold: true, color: CYAN, align: "center", valign: "middle", margin: 0 });
});
// repeat hint
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: startX + cw * 1.5, y: 6.25, w: cw, h: 0.55, rectRadius: 0.27, fill: { color: PANEL }, line: { color: EDGE, width: 1 } });
s.addText("↻  반복하며 도감 완성", { x: startX + cw * 1.5, y: 6.25, w: cw, h: 0.55, fontFace: F, fontSize: 13, bold: true, color: TEAL, align: "center", valign: "middle", margin: 0 });

// ════════════════════════════════════════════════════
// 5. 기능 ① 손전등 스캔
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Feature 01"); heading(s, "손전등 스캔 시스템"); pageNo(s, 5);
const f1 = [
  ["조준 & 유지", "물고기에 손전등 콘을 겨누고 트리거를 누르고 있으면 진행도가 차오름"],
  ["락온(Lock-on)", "한번 포착하면 물고기가 도망쳐 시야를 벗어나도 스캔이 계속 진행"],
  ["물고기별 스캔 시간", "데이터테이블의 ScanTime 값으로 종마다 난이도(소요 시간)가 다름"],
  ["완료 & 피드백", "진행도 UI·루프 사운드, 100% 도달 시 도감 등록 + 완료음"],
];
f1.forEach((d, i) => {
  const y = 1.95 + i * 1.18;
  s.addShape(p.shapes.OVAL, { x: 0.8, y: y + 0.07, w: 0.34, h: 0.34, fill: { color: CORAL }, line: { type: "none" } });
  s.addText(d[0], { x: 1.35, y: y, w: 6.0, h: 0.45, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.43, w: 6.1, h: 0.65, fontFace: F, fontSize: 13.5, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
// right mock: cone + progress
card(s, 8.0, 1.95, 4.6, 4.6, DEEP);
s.addText("SCAN", { x: 8.0, y: 2.2, w: 4.6, h: 0.4, fontFace: F, fontSize: 13, bold: true, color: CYAN, align: "center", charSpacing: 3, margin: 0 });
// flashlight cone (triangle via rotated shapes) — use two ovals as light + target
s.addShape(p.shapes.OVAL, { x: 8.55, y: 2.75, w: 0.5, h: 0.5, fill: { color: GOLD }, line: { type: "none" } });
s.addText("손전등", { x: 8.2, y: 3.28, w: 1.2, h: 0.3, fontFace: F, fontSize: 10, color: "9FC4D4", align: "center", margin: 0 });
s.addShape(p.shapes.OVAL, { x: 11.2, y: 3.7, w: 0.7, h: 0.45, fill: { color: CORAL }, line: { type: "none" } });
s.addText("물고기", { x: 10.9, y: 4.15, w: 1.3, h: 0.3, fontFace: F, fontSize: 10, color: "9FC4D4", align: "center", margin: 0 });
s.addShape(p.shapes.LINE, { x: 9.05, y: 3.0, w: 2.3, h: 1.0, line: { color: GOLD, width: 1.5, dashType: "dash" } });
// progress bar
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.45, y: 5.25, w: 3.7, h: 0.42, rectRadius: 0.21, fill: { color: "0A2E47" }, line: { color: TEAL, width: 1 } });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.45, y: 5.25, w: 2.55, h: 0.42, rectRadius: 0.21, fill: { color: CYAN }, line: { type: "none" } });
s.addText("스캔 진행 중...  68%", { x: 8.45, y: 5.8, w: 3.7, h: 0.35, fontFace: F, fontSize: 12, bold: true, color: "CFE6EE", align: "center", margin: 0 });

// ════════════════════════════════════════════════════
// 6. 기능 ② 소나 탐지
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Feature 02"); heading(s, "소나(Sonar) 탐지"); pageNo(s, 6);
const f2 = [
  ["Y버튼으로 가동", "왼손 컨트롤러로 소나를 펼쳐 주변 해역을 한 번에 탐지"],
  ["생물 위치 안내", "감지된 물고기의 방향을 화살표/표시로 알려줘 탐험을 도움"],
  ["전용 사운드", "가동음(스윕)과 감지음을 분리 → 탐지 순간의 손맛과 몰입"],
];
f2.forEach((d, i) => {
  const y = 2.1 + i * 1.45;
  card(s, 0.72, y, 6.0, 1.28);
  s.addShape(p.shapes.OVAL, { x: 1.0, y: y + 0.33, w: 0.6, h: 0.6, fill: { color: BLUE }, line: { type: "none" } });
  s.addText(String(i + 1), { x: 1.0, y: y + 0.33, w: 0.6, h: 0.6, fontFace: F, fontSize: 18, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
  s.addText(d[0], { x: 1.8, y: y + 0.2, w: 4.7, h: 0.45, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 1.8, y: y + 0.66, w: 4.7, h: 0.55, fontFace: F, fontSize: 13.5, color: MUTE, margin: 0 });
});
// right: sonar ping concentric circles
card(s, 7.1, 2.1, 5.5, 4.55, DEEP);
const ccx = 7.1 + 5.5 / 2, ccy = 2.1 + 4.55 / 2;
[2.7, 2.0, 1.3].forEach((r, i) => {
  s.addShape(p.shapes.OVAL, { x: ccx - r, y: ccy - r, w: r * 2, h: r * 2, fill: { type: "none" }, line: { color: CYAN, width: 2, transparency: i * 22 } });
});
s.addShape(p.shapes.OVAL, { x: ccx - 0.22, y: ccy - 0.22, w: 0.44, h: 0.44, fill: { color: GOLD }, line: { type: "none" } });
// detected fish dots
[[ccx + 1.6, ccy - 1.0], [ccx - 1.9, ccy + 0.7], [ccx + 0.8, ccy + 1.7]].forEach(d => {
  s.addShape(p.shapes.OVAL, { x: d[0] - 0.13, y: d[1] - 0.13, w: 0.26, h: 0.26, fill: { color: CORAL }, line: { type: "none" } });
});
s.addText("SONAR PING", { x: 7.1, y: 6.05, w: 5.5, h: 0.35, fontFace: F, fontSize: 12, bold: true, color: CYAN, align: "center", charSpacing: 2, margin: 0 });

// ════════════════════════════════════════════════════
// 7. 기능 ③ 도감 시스템
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Feature 03"); heading(s, "데이터 기반 도감 시스템"); pageNo(s, 7);
const f3 = [
  ["데이터테이블 구동", "종별 이름·수심·스테이지·스캔시간·모델/애니를 표 한 줄로 관리"],
  ["영구 수집 저장", "GameInstanceSubsystem 에 기록 → 도감을 닫았다 열어도 유지"],
  ["스마트 매칭", "이름 대소문자·접두어·오타까지 보정해 정확한 종으로 등록"],
  ["홀로그램 표시", "수집한 종을 홀로그램 도감에 한글 폰트로 출력"],
];
f3.forEach((d, i) => {
  const y = 1.95 + i * 1.2;
  s.addShape(p.shapes.OVAL, { x: 0.8, y: y + 0.06, w: 0.34, h: 0.34, fill: { color: TEAL }, line: { type: "none" } });
  s.addText(d[0], { x: 1.35, y: y, w: 6.1, h: 0.45, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.43, w: 6.2, h: 0.65, fontFace: F, fontSize: 13.5, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
// right: encyclopedia card mock
card(s, 8.1, 1.95, 4.5, 4.6, WHITE);
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.35, y: 2.2, w: 4.0, h: 1.9, rectRadius: 0.08, fill: { color: PANEL }, line: { color: EDGE, width: 1 } });
s.addShape(p.shapes.OVAL, { x: 9.9, y: 2.55, w: 0.95, h: 0.95, fill: { color: TEAL }, line: { type: "none" } });
s.addText("No.07", { x: 8.55, y: 2.4, w: 1.5, h: 0.4, fontFace: F, fontSize: 13, bold: true, color: CORAL, margin: 0 });
s.addText("Mandarine Fish", { x: 8.55, y: 4.25, w: 3.6, h: 0.4, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
[["수심", "12 m"], ["스테이지", "2"], ["스캔시간", "3.0초"], ["수집", "완료 ✓"]].forEach((r, i) => {
  const yy = 4.8 + i * 0.42;
  s.addText(r[0], { x: 8.55, y: yy, w: 1.6, h: 0.36, fontFace: F, fontSize: 13, color: MUTE, margin: 0 });
  s.addText(r[1], { x: 10.2, y: yy, w: 2.1, h: 0.36, fontFace: F, fontSize: 13, bold: true, color: i === 3 ? "1C9B7A" : INK, margin: 0 });
});

// ════════════════════════════════════════════════════
// 8. 기능 ④ 살아있는 바다
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Feature 04"); heading(s, "살아있는 바다 — 물고기 AI & 수심"); pageNo(s, 8);
const f4 = [
  ["자연스러운 유영", "전진·랜덤 방향 전환(wander)으로 살아있는 움직임"],
  ["벽 회피", "앞을 미리 감지해 바위·지형에 박히지 않고 부드럽게 회피"],
  ["수심대 가두기 & 도망", "종마다 사는 깊이대 유지, 스캔 당하면 빠르게 도망"],
  ["수심 기반 자동 스폰", "데이터의 수심값대로 깊은 종은 더 깊은 곳에 자동 배치"],
];
f4.forEach((d, i) => {
  const y = 1.95 + i * 1.2;
  s.addShape(p.shapes.OVAL, { x: 0.8, y: y + 0.06, w: 0.34, h: 0.34, fill: { color: ["12869A", "0E4D6E", "FF7A59", "1C9B7A"][i] }, line: { type: "none" } });
  s.addText(d[0], { x: 1.35, y: y, w: 6.0, h: 0.45, fontFace: F, fontSize: 17, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.43, w: 6.1, h: 0.65, fontFace: F, fontSize: 13.5, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
// right: depth bands
const bx = 8.1, bw = 4.5, by = 1.95;
const bands = [["얕은 수심", "85B9D6", ["Bluetang", "Clione"]], ["중간 수심", "3E84A8", ["Mandarine", "Lionfish"]], ["깊은 수심", "0E3A55", ["Shark", "Whale"]]];
bands.forEach((b, i) => {
  const yy = by + i * 1.5;
  s.addShape(p.shapes.RECTANGLE, { x: bx, y: yy, w: bw, h: 1.45, fill: { color: b[1] }, line: { type: "none" } });
  s.addText(b[0], { x: bx + 0.25, y: yy + 0.15, w: 2.5, h: 0.4, fontFace: F, fontSize: 14, bold: true, color: WHITE, margin: 0 });
  s.addText(b[2].join("  ·  "), { x: bx + 0.25, y: yy + 0.62, w: 4.0, h: 0.4, fontFace: F, fontSize: 12, color: "EAF3F7", margin: 0 });
  // fish dots
  for (let k = 0; k < 3; k++) s.addShape(p.shapes.OVAL, { x: bx + 3.4 + k * 0.3, y: yy + 0.95, w: 0.16, h: 0.16, fill: { color: i === 2 ? CORAL : CYAN }, line: { type: "none" } });
});
s.addText("← 깊어질수록 다른 종이 산다", { x: bx, y: by + 4.5, w: bw, h: 0.35, fontFace: F, fontSize: 11, italic: true, color: MUTE, align: "center", margin: 0 });

// ════════════════════════════════════════════════════
// 9. 사운드 & 몰입
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Audio"); heading(s, "사운드 & 몰입 연출"); pageNo(s, 9);
const au = [
  ["배경음 자동 정지/재개", "스캔·소나 중에는 BGM을 멈췄다가, 끝나면 멈춘 지점부터 이어 재생 → 행동에 집중"],
  ["스캔 사운드", "스캔 중 루프음 + 완료 시 1회 효과음으로 명확한 피드백"],
  ["소나 사운드", "가동음 / 감지음을 분리해 탐지 단계마다 다른 청각 신호"],
  ["공간감", "효과음은 2D·3D를 구분해, 안내음은 또렷하게 / 환경음은 입체적으로"],
];
au.forEach((d, i) => {
  const y = 1.95 + i * 1.18;
  card(s, 0.72, y, 7.3, 1.02);
  s.addShape(p.shapes.OVAL, { x: 1.0, y: y + 0.26, w: 0.5, h: 0.5, fill: { color: [TEAL, CORAL, BLUE, "1C9B7A"][i] }, line: { type: "none" } });
  s.addText(d[0], { x: 1.7, y: y + 0.13, w: 6.1, h: 0.42, fontFace: F, fontSize: 16, bold: true, color: INK, margin: 0 });
  s.addText(d[1], { x: 1.7, y: y + 0.55, w: 6.2, h: 0.42, fontFace: F, fontSize: 12.5, color: MUTE, margin: 0 });
});
// right: equalizer bars
card(s, 8.3, 1.95, 4.3, 4.7, DEEP);
const eqx = 8.7, eqBase = 5.9, heights = [1.2, 2.3, 1.7, 3.0, 2.0, 2.7, 1.4, 2.4];
heights.forEach((hh, i) => {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: eqx + i * 0.46, y: eqBase - hh, w: 0.32, h: hh, rectRadius: 0.05, fill: { color: i % 2 ? CYAN : TEAL }, line: { type: "none" } });
});
s.addText("몰입을 만드는 소리", { x: 8.3, y: 6.1, w: 4.3, h: 0.4, fontFace: F, fontSize: 13, bold: true, color: CYAN, align: "center", margin: 0 });

// ════════════════════════════════════════════════════
// 10. 기술 스택 & 아키텍처
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Tech"); heading(s, "기술 스택 & 구조"); pageNo(s, 10);
// stack chips row
const stack = ["Unreal Engine 5.7", "OpenXR (VR)", "C++ + Blueprint", "DataTable", "GameInstance Subsystem"];
stack.forEach((t, i) => {
  const x = 0.72 + i * 2.46;
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y: 1.85, w: 2.32, h: 0.62, rectRadius: 0.31, fill: { color: PANEL }, line: { color: TEAL, width: 1 } });
  s.addText(t, { x, y: 1.85, w: 2.32, h: 0.62, fontFace: F, fontSize: 11.5, bold: true, color: BLUE, align: "center", valign: "middle", margin: 0 });
});
// architecture boxes
function archBox(x, y, w, h, title, items, col) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.08, fill: { color: WHITE }, line: { color: EDGE, width: 1 }, shadow: shadow() });
  s.addShape(p.shapes.RECTANGLE, { x, y, w, h: 0.5, fill: { color: col }, line: { type: "none" } });
  s.addText(title, { x: x + 0.15, y: y, w: w - 0.3, h: 0.5, fontFace: F, fontSize: 14, bold: true, color: WHITE, valign: "middle", margin: 0 });
  s.addText(items.map(t => ({ text: t, options: { bullet: { code: "2022" }, breakLine: true, color: INK } })),
    { x: x + 0.25, y: y + 0.62, w: w - 0.45, h: h - 0.75, fontFace: F, fontSize: 12.5, color: INK, lineSpacingMultiple: 1.15, valign: "top", margin: 0 });
}
archBox(0.72, 2.85, 3.9, 3.7, "Player", ["XRPawn (VR 폰)", "이동 · 대시", "손전등 스캔", "소나 · 수심 표시"], TEAL);
archBox(4.78, 2.85, 4.0, 3.7, "Encyclopedia", ["EncyclopediaLibrary", "EncyclopediaSubsystem", "FishSpawner", "FishSwimComponent"], CORAL);
archBox(8.94, 2.85, 3.66, 3.7, "Systems", ["Audio (BGM)", "Menu / GameMode", "Settings", "Core 프레임워크"], BLUE);
// connectors
s.addShape(p.shapes.LINE, { x: 4.62, y: 4.7, w: 0.16, h: 0, line: { color: MUTE, width: 1.5 } });
s.addShape(p.shapes.LINE, { x: 8.78, y: 4.7, w: 0.16, h: 0, line: { color: MUTE, width: 1.5 } });

// ════════════════════════════════════════════════════
// 11. 핵심 알고리즘
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Algorithm"); heading(s, "핵심 알고리즘"); pageNo(s, 11);
// LEFT: fuzzy name matching
card(s, 0.72, 1.85, 5.9, 3.7, WHITE);
s.addShape(p.shapes.OVAL, { x: 1.0, y: 2.12, w: 0.55, h: 0.55, fill: { color: TEAL }, line: { type: "none" } });
s.addText("1", { x: 1.0, y: 2.12, w: 0.55, h: 0.55, fontFace: F, fontSize: 18, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
s.addText("퍼지 이름 매칭", { x: 1.7, y: 2.05, w: 4.7, h: 0.45, fontFace: F, fontSize: 19, bold: true, color: INK, margin: 0 });
s.addText("레벤슈타인 편집거리 · 오타/대소문자/접두어 보정", { x: 1.7, y: 2.52, w: 4.85, h: 0.35, fontFace: F, fontSize: 11.5, color: MUTE, margin: 0 });
s.addText([
  { text: "① 완전 일치", options: { breakLine: true, color: INK } },
  { text: "② 정규화 일치 (소문자·영숫자)", options: { breakLine: true, color: INK } },
  { text: "③ 접두 / 부분 일치", options: { breakLine: true, color: INK } },
  { text: "④ 편집거리 ≤ 1  (오타 1글자, 유일할 때)", options: { color: CORAL, bold: true } },
], { x: 1.05, y: 3.05, w: 5.3, h: 1.6, fontFace: F, fontSize: 14, lineSpacingMultiple: 1.3, margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 1.05, y: 4.78, w: 5.3, h: 0.6, rectRadius: 0.1, fill: { color: NAVY }, line: { type: "none" } });
s.addText([
  { text: "\"madarine\"  →  ", options: { color: "9FC4D4" } },
  { text: "mandarine", options: { color: GOLD, bold: true } },
  { text: "   ✓", options: { color: "5FE0A0", bold: true } },
], { x: 1.25, y: 4.78, w: 5.0, h: 0.6, fontFace: "Consolas", fontSize: 14, valign: "middle", margin: 0 });

// RIGHT: angular-radius cone targeting
card(s, 6.72, 1.85, 5.9, 3.7, WHITE);
s.addShape(p.shapes.OVAL, { x: 7.0, y: 2.12, w: 0.55, h: 0.55, fill: { color: CORAL }, line: { type: "none" } });
s.addText("2", { x: 7.0, y: 2.12, w: 0.55, h: 0.55, fontFace: F, fontSize: 18, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
s.addText("각반경 콘 판정", { x: 7.7, y: 2.05, w: 4.7, h: 0.45, fontFace: F, fontSize: 19, bold: true, color: INK, margin: 0 });
s.addText("벡터 기하 기반 스캔 타겟팅", { x: 7.7, y: 2.52, w: 4.85, h: 0.35, fontFace: F, fontSize: 11.5, color: MUTE, margin: 0 });
const apx = 7.35, apy = 3.55;
s.addShape(p.shapes.LINE, { x: apx, y: apy, w: 2.3, h: -0.6, line: { color: GOLD, width: 1.5 } });
s.addShape(p.shapes.LINE, { x: apx, y: apy, w: 2.3, h: 0.6, line: { color: GOLD, width: 1.5 } });
s.addShape(p.shapes.OVAL, { x: apx - 0.13, y: apy - 0.13, w: 0.26, h: 0.26, fill: { color: GOLD }, line: { type: "none" } });
s.addShape(p.shapes.OVAL, { x: apx + 2.3, y: apy - 0.19, w: 0.38, h: 0.38, fill: { color: CORAL }, line: { type: "none" } });
s.addText("손전등", { x: apx - 0.35, y: apy + 0.18, w: 1.2, h: 0.3, fontFace: F, fontSize: 10, color: MUTE, margin: 0 });
s.addText("물고기", { x: apx + 1.95, y: apy + 0.18, w: 1.3, h: 0.3, fontFace: F, fontSize: 10, color: MUTE, align: "center", margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 7.05, y: 4.5, w: 5.25, h: 0.88, rectRadius: 0.1, fill: { color: NAVY }, line: { type: "none" } });
s.addText([
  { text: "θ = acos( Forward · toFish )", options: { breakLine: true, color: CYAN } },
  { text: "허용각 += asin( r / dist )", options: { color: GOLD } },
], { x: 7.25, y: 4.5, w: 5.0, h: 0.88, fontFace: "Consolas", fontSize: 12.5, valign: "middle", lineSpacingMultiple: 1.15, margin: 0 });

// BOTTOM strip: AI steering
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 0.72, y: 5.78, w: 11.9, h: 0.95, rectRadius: 0.1, fill: { color: PANEL }, line: { color: EDGE, width: 1 } });
s.addShape(p.shapes.OVAL, { x: 1.0, y: 6.0, w: 0.5, h: 0.5, fill: { color: BLUE }, line: { type: "none" } });
s.addText("3", { x: 1.0, y: 6.0, w: 0.5, h: 0.5, fontFace: F, fontSize: 16, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
s.addText([
  { text: "물고기 AI — 예측형 회피 + 스티어링:   ", options: { bold: true, color: INK } },
  { text: "앞을 미리 스윕해 충돌 전 법선 방향으로 회피 · 목표 수심 비례제어 · 등속 보간 회전", options: { color: MUTE } },
], { x: 1.7, y: 5.88, w: 10.7, h: 0.75, fontFace: F, fontSize: 13.5, valign: "middle", lineSpacingMultiple: 1.1, margin: 0 });

// ════════════════════════════════════════════════════
// 12. 콘텐츠
// ════════════════════════════════════════════════════
s = p.addSlide(); lightBG(s); kicker(s, "Content"); heading(s, "콘텐츠 — 해양 생물 & 환경"); pageNo(s, 12);
s.addText("현재 12종의 해양 생물과 광활한 수중 환경을 제작했습니다.", { x: 0.72, y: 1.7, w: 12, h: 0.45, fontFace: F, fontSize: 15, color: MUTE, margin: 0 });
const fish = ["Blobfish", "Bluetang", "Clione", "Crownfish", "Lionfish", "Lure", "Mandarine", "Seahorse", "Shark", "Turtle", "Whale", "Octopus"];
const gx = 0.72, gy = 2.35, gw = 2.95, gh = 0.78, gxgap = 0.13, gygap = 0.13;
fish.forEach((f, i) => {
  const col = i % 4, row = Math.floor(i / 4);
  const x = gx + col * (gw + gxgap), y = gy + row * (gh + gygap);
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w: gw, h: gh, rectRadius: 0.08, fill: { color: PANEL2 }, line: { color: EDGE, width: 1 } });
  s.addShape(p.shapes.OVAL, { x: x + 0.18, y: y + 0.19, w: 0.4, h: 0.4, fill: { color: [TEAL, BLUE, CORAL][i % 3] }, line: { type: "none" } });
  s.addText(String(i + 1), { x: x + 0.18, y: y + 0.19, w: 0.4, h: 0.4, fontFace: F, fontSize: 13, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0 });
  s.addText(f, { x: x + 0.72, y: y, w: gw - 0.8, h: gh, fontFace: F, fontSize: 14.5, bold: true, color: INK, valign: "middle", margin: 0 });
});
// environment strip
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 0.72, y: 5.7, w: 11.9, h: 1.05, rectRadius: 0.1, fill: { color: DEEP }, line: { type: "none" }, shadow: shadow() });
s.addText("수중 환경", { x: 1.05, y: 5.85, w: 2, h: 0.4, fontFace: F, fontSize: 14, bold: true, color: GOLD, margin: 0 });
s.addText("산호초 군락  ·  해초 숲  ·  침몰한 고대 유적  ·  거대 고래뼈  ·  심해 기지(DeepWaterStation)", { x: 1.05, y: 6.25, w: 11.3, h: 0.4, fontFace: F, fontSize: 14, color: "CFE6EE", margin: 0 });

// ════════════════════════════════════════════════════
// 12. 시연 영상
// ════════════════════════════════════════════════════
s = p.addSlide(); darkBG(s);
s.addShape(p.shapes.OVAL, { x: W / 2 - 0.85, y: 2.15, w: 1.7, h: 1.7, fill: { color: CORAL }, line: { type: "none" }, shadow: shadow() });
// play triangle
s.addText("▶", { x: W / 2 - 0.7, y: 2.15, w: 1.7, h: 1.7, fontFace: F, fontSize: 40, color: WHITE, align: "center", valign: "middle", margin: 0 });
s.addText("시연 영상", { x: 0, y: 4.1, w: W, h: 0.8, fontFace: F, fontSize: 44, bold: true, color: WHITE, align: "center", margin: 0 });
s.addText("약 2분  ·  탐험 → 발견 → 스캔 → 도감 등록의 실제 플레이", { x: 0, y: 5.0, w: W, h: 0.5, fontFace: F, fontSize: 18, color: "CFE6EE", align: "center", margin: 0 });
s.addText("▶  영상 재생", { x: W / 2 - 1.5, y: 5.8, w: 3, h: 0.55, fontFace: F, fontSize: 14, bold: true, color: NAVY, align: "center", valign: "middle", fill: { color: GOLD }, margin: 0 });

// ════════════════════════════════════════════════════
// 13. 마무리
// ════════════════════════════════════════════════════
s = p.addSlide(); darkBG(s);
s.addText("THANK YOU", { x: 0.95, y: 1.5, w: 11, h: 0.5, fontFace: F, fontSize: 14, bold: true, color: CYAN, charSpacing: 4, margin: 0 });
s.addText("바닷속의 발견을 도감에 담다", { x: 0.92, y: 2.0, w: 11.5, h: 1.0, fontFace: F, fontSize: 40, bold: true, color: WHITE, margin: 0 });
// summary + future two columns
s.addText("핵심 요약", { x: 1.0, y: 3.45, w: 5.5, h: 0.4, fontFace: F, fontSize: 17, bold: true, color: GOLD, margin: 0 });
s.addText([
  { text: "VR 수중 탐험 + 손전등 스캔 / 소나 도감 수집", options: { bullet: { code: "2022" }, breakLine: true } },
  { text: "데이터 기반 도감 · 영구 저장으로 확장성 확보", options: { bullet: { code: "2022" }, breakLine: true } },
  { text: "반응하는 물고기 AI로 살아있는 생태계 구현", options: { bullet: { code: "2022" } } },
], { x: 1.0, y: 3.9, w: 5.6, h: 2.2, fontFace: F, fontSize: 14.5, color: "CFE6EE", lineSpacingMultiple: 1.35, margin: 0 });
s.addText("향후 계획", { x: 7.1, y: 3.45, w: 5.5, h: 0.4, fontFace: F, fontSize: 17, bold: true, color: GOLD, margin: 0 });
s.addText([
  { text: "해양 생물 종 추가 및 희귀종 도입", options: { bullet: { code: "2022" }, breakLine: true } },
  { text: "미션 / 퀘스트, 사진 모드 등 콘텐츠 확장", options: { bullet: { code: "2022" }, breakLine: true } },
  { text: "사운드 · 비주얼 폴리싱으로 몰입감 강화", options: { bullet: { code: "2022" } } },
], { x: 7.1, y: 3.9, w: 5.5, h: 2.2, fontFace: F, fontSize: 14.5, color: "CFE6EE", lineSpacingMultiple: 1.35, margin: 0 });
s.addText("SeaProject  ·  Unreal Engine 5.7  ·  OpenXR", { x: 0, y: 6.7, w: W, h: 0.4, fontFace: F, fontSize: 12, color: MUTE, align: "center", margin: 0 });

p.writeFile({ fileName: "C:/Users/pch31/Documents/GitHub/Seaproject/발표자료/SeaProject_발표자료.pptx" }).then(f => console.log("SAVED:", f));
