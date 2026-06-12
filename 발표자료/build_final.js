const pptxgen = require("pptxgenjs");
const p = new pptxgen();
p.layout = "LAYOUT_WIDE"; const W = 13.33, H = 7.5;
p.author = "DEEP DIVE VR Team"; p.title = "DEEP DIVE VR 기말 발표";

// ── Deep-sea palette (중간발표 톤 계승) ──
const BG0 = "01101E", BG1 = "082039", BG2 = "08365C", CARD = "0E3356", CARD2 = "0B2A47", BORDER = "1E5C86";
const CYAN = "00D3FF", CYAN2 = "53DCFF", MINT = "00E2BE", GOLD = "FFCC4D", PINK = "F472B6", PURPLE = "9A6CFF";
const WHITE = "F7FCFF", TXT = "C2ECFF", MUTE = "9FC4DC", DIM = "6F93AC";
const NK = "Noto Sans KR", BLACK = "Arial Black";
const sh = () => ({ type: "outer", color: "000814", blur: 10, offset: 3, angle: 90, opacity: 0.35 });
const ACCENTS = [CYAN, MINT, GOLD, PINK, PURPLE, CYAN2];

function bg(s) {
  s.background = { color: BG0 };
  s.addShape(p.shapes.OVAL, { x: -2.5, y: -4.5, w: 12, h: 9, fill: { color: BG2, transparency: 62 }, line: { type: "none" } });
  s.addShape(p.shapes.OVAL, { x: 8.0, y: 3.8, w: 9.5, h: 8.5, fill: { color: "0A3A5E", transparency: 74 }, line: { type: "none" } });
}
function sec(s, num, kr, en) {
  s.addText(num, { x: 0.72, y: 0.46, w: 1.15, h: 0.82, fontFace: BLACK, fontSize: 30, color: CYAN, valign: "middle", margin: 0 });
  s.addText(kr, { x: 1.92, y: 0.46, w: 10.7, h: 0.62, fontFace: NK, fontSize: 26, bold: true, color: WHITE, valign: "middle", margin: 0 });
  s.addText(en.toUpperCase(), { x: 1.95, y: 1.12, w: 10.6, h: 0.32, fontFace: NK, fontSize: 11, bold: true, color: MINT, charSpacing: 3, margin: 0 });
}
function card(s, x, y, w, h, fill) {
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.09, fill: { color: fill || CARD }, line: { color: BORDER, width: 1 }, shadow: sh() });
}
function foot(s, n) {
  s.addText("DEEP DIVE VR", { x: 0.72, y: H - 0.5, w: 4, h: 0.3, fontFace: NK, fontSize: 9, bold: true, color: DIM, charSpacing: 2, margin: 0 });
  s.addText(String(n).padStart(2, "0"), { x: W - 1.0, y: H - 0.5, w: 0.6, h: 0.3, fontFace: BLACK, fontSize: 10, color: DIM, align: "right", margin: 0 });
}
function dot(s, x, y, c, d = 0.34) { s.addShape(p.shapes.OVAL, { x, y, w: d, h: d, fill: { color: c }, line: { type: "none" } }); }

// ════════ 1. TITLE ════════
let s = p.addSlide(); bg(s);
s.addText("VR DEEP SEA EXPLORATION", { x: 0.9, y: 1.55, w: 11, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: CYAN, charSpacing: 5, margin: 0 });
s.addText("DEEP DIVE VR", { x: 0.84, y: 2.0, w: 12, h: 1.7, fontFace: BLACK, fontSize: 70, color: WHITE, margin: 0 });
s.addText("VR 심해 탐사 시뮬레이션", { x: 0.9, y: 3.75, w: 11, h: 0.6, fontFace: NK, fontSize: 24, bold: true, color: CYAN2, margin: 0 });
s.addText("탐사하고 · 소나로 발견하고 · 스캔해 홀로그램 도감을 채우다", { x: 0.92, y: 4.4, w: 11.5, h: 0.45, fontFace: NK, fontSize: 15, color: TXT, margin: 0 });
["2026 XR UI/UX 프로그래밍", "기말 발표", "TEAM 박찬홍 (팀장·기획·프로그래밍)"].forEach((c, i) => {
  const cx = 0.9 + i * 3.7;
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: cx, y: 5.35, w: 3.5, h: 0.58, rectRadius: 0.29, fill: { color: CARD2 }, line: { color: BORDER, width: 1 } });
  s.addText(c, { x: cx, y: 5.35, w: 3.5, h: 0.58, fontFace: NK, fontSize: 11.5, bold: true, color: i === 1 ? GOLD : CYAN2, align: "center", valign: "middle", margin: 0 });
});
s.addText("발표 8분  +  시연 영상 2분", { x: 0.92, y: 6.45, w: 8, h: 0.4, fontFace: NK, fontSize: 12, color: DIM, margin: 0 });

// ════════ 2. 01 프로젝트 개요 ════════
s = p.addSlide(); bg(s); sec(s, "01", "프로젝트 개요", "Project Overview"); foot(s, 2);
card(s, 0.72, 1.85, 11.9, 1.3, BG1);
s.addText([
  { text: "플레이어는 VR로 심해를 탐사하며, ", options: { color: TXT } },
  { text: "소나로 생물을 발견하고 손전등으로 스캔", options: { color: CYAN, bold: true } },
  { text: "해 ", options: { color: TXT } },
  { text: "홀로그램 도감", options: { color: GOLD, bold: true } },
  { text: "을 완성하는 심해 탐사 시뮬레이션입니다.", options: { color: TXT } },
], { x: 1.1, y: 1.85, w: 11.1, h: 1.3, fontFace: NK, fontSize: 18, valign: "middle", lineSpacingMultiple: 1.25, margin: 0 });
const pil = [["심해의 공포와 경이", "빛이 닿지 않는 극한 환경을 VR로 직접 탐사하는 몰입 경험", CYAN],
["발견과 수집의 재미", "소나 탐지 → 스캔 → 도감 등록으로 이어지는 탐사 보상 루프", MINT],
["다이제틱 몰입 UI", "손목 수심계·홀로그램 도감 등 UI가 게임 세계 안에 존재", GOLD]];
pil.forEach((c, i) => {
  const x = 0.72 + i * 4.06; card(s, x, 3.5, 3.82, 3.3);
  s.addShape(p.shapes.RECTANGLE, { x, y: 3.5, w: 0.1, h: 3.3, fill: { color: c[2] }, line: { type: "none" } });
  s.addText(String(i + 1).padStart(2, "0"), { x: x + 0.3, y: 3.75, w: 2, h: 0.6, fontFace: BLACK, fontSize: 26, color: c[2], margin: 0 });
  s.addText(c[0], { x: x + 0.3, y: 4.5, w: 3.3, h: 0.5, fontFace: NK, fontSize: 17, bold: true, color: WHITE, margin: 0 });
  s.addText(c[1], { x: x + 0.3, y: 5.05, w: 3.3, h: 1.6, fontFace: NK, fontSize: 13, color: MUTE, lineSpacingMultiple: 1.25, margin: 0 });
});

// ════════ 3. 02 핵심 플레이 루프 ════════
s = p.addSlide(); bg(s); sec(s, "02", "핵심 플레이 루프", "Core Gameplay Loop"); foot(s, 3);
s.addText("탐사 → 발견 → 스캔 → 도감. 이 사이클의 반복이 게임의 핵심 재미입니다.", { x: 0.72, y: 1.62, w: 12, h: 0.45, fontFace: NK, fontSize: 15, color: MUTE, margin: 0 });
const loop = [["🌊", "심해 이동", "수중 추진기로\n3D 심해 탐색", CYAN], ["📡", "소나 탐지", "주변 생물의\n위치를 포착", MINT], ["🔦", "손전등 스캔", "겨눠서 진행도를\n채워 포획", GOLD], ["📖", "도감 등록", "수집을 영구 저장\n(다시 와도 유지)", CYAN2]];
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

// ════════ 4. 03 스캔 시스템 ════════
s = p.addSlide(); bg(s); sec(s, "03", "긴장감 있는 스캔 시스템", "Scan Mechanic"); foot(s, 4);
const f1 = [["조준 & 유지", "손전등 콘을 겨누고 트리거를 누르고 있으면 진행도가 차오름"],
["락온(Lock-on)", "한번 포착하면 생물이 도망쳐 시야를 벗어나도 스캔 지속"],
["생물별 스캔 시간", "데이터테이블 값으로 종마다 난이도(소요 시간)가 다름"],
["완료 피드백", "진행도 UI·루프 사운드, 100%에서 도감 등록 + 완료음"]];
f1.forEach((d, i) => {
  const y = 1.95 + i * 1.18; dot(s, 0.8, y + 0.07, CYAN);
  s.addText(d[0], { x: 1.35, y: y, w: 6.0, h: 0.45, fontFace: NK, fontSize: 17, bold: true, color: WHITE, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.45, w: 6.2, h: 0.6, fontFace: NK, fontSize: 13, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
card(s, 8.05, 1.95, 4.55, 4.6, CARD2);
s.addText("SCAN", { x: 8.05, y: 2.2, w: 4.55, h: 0.35, fontFace: NK, fontSize: 12, bold: true, color: CYAN, align: "center", charSpacing: 3, margin: 0 });
dot(s, 8.6, 2.85, GOLD, 0.46); s.addText("손전등", { x: 8.25, y: 3.35, w: 1.2, h: 0.3, fontFace: NK, fontSize: 10, color: MUTE, align: "center", margin: 0 });
s.addShape(p.shapes.LINE, { x: 9.05, y: 3.08, w: 2.2, h: 0.95, line: { color: GOLD, width: 1.5, dashType: "dash" } });
dot(s, 11.2, 3.75, PINK, 0.5); s.addText("생물", { x: 10.95, y: 4.27, w: 1.2, h: 0.3, fontFace: NK, fontSize: 10, color: MUTE, align: "center", margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.4, y: 5.2, w: 3.85, h: 0.4, rectRadius: 0.2, fill: { color: "06223B" }, line: { color: BORDER, width: 1 } });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.4, y: 5.2, w: 2.6, h: 0.4, rectRadius: 0.2, fill: { color: CYAN }, line: { type: "none" } });
s.addText("스캔 진행 중...  68%", { x: 8.4, y: 5.75, w: 3.85, h: 0.35, fontFace: NK, fontSize: 12, bold: true, color: TXT, align: "center", margin: 0 });

// ════════ 5. 04 소나 탐지 ════════
s = p.addSlide(); bg(s); sec(s, "04", "소나(Sonar) 탐지", "Sonar Detection"); foot(s, 5);
const f2 = [["버튼으로 가동", "왼손으로 소나를 펼쳐 주변 해역을 한 번에 탐지"], ["생물 위치 안내", "감지된 생물의 방향을 표시해 탐사를 도움"], ["전용 사운드", "가동음·감지음을 분리해 탐지 단계마다 다른 청각 신호"]];
f2.forEach((d, i) => {
  const y = 2.1 + i * 1.45; card(s, 0.72, y, 6.0, 1.28);
  s.addShape(p.shapes.OVAL, { x: 1.0, y: y + 0.34, w: 0.6, h: 0.6, fill: { color: MINT }, line: { type: "none" } });
  s.addText(String(i + 1), { x: 1.0, y: y + 0.34, w: 0.6, h: 0.6, fontFace: BLACK, fontSize: 16, color: BG0, align: "center", valign: "middle", margin: 0 });
  s.addText(d[0], { x: 1.8, y: y + 0.21, w: 4.7, h: 0.45, fontFace: NK, fontSize: 17, bold: true, color: WHITE, margin: 0 });
  s.addText(d[1], { x: 1.8, y: y + 0.67, w: 4.7, h: 0.5, fontFace: NK, fontSize: 13, color: MUTE, margin: 0 });
});
card(s, 7.1, 2.1, 5.5, 4.5, CARD2);
const cx = 7.1 + 2.75, cyy = 2.1 + 2.25;
[2.55, 1.85, 1.15].forEach((r, i) => s.addShape(p.shapes.OVAL, { x: cx - r, y: cyy - r, w: r * 2, h: r * 2, fill: { type: "none" }, line: { color: CYAN, width: 2, transparency: i * 24 } }));
dot(s, cx - 0.2, cyy - 0.2, GOLD, 0.4);
[[cx + 1.5, cyy - 0.95], [cx - 1.8, cyy + 0.65], [cx + 0.75, cyy + 1.6]].forEach(d => dot(s, d[0] - 0.12, d[1] - 0.12, PINK, 0.24));
s.addText("SONAR PING", { x: 7.1, y: 6.0, w: 5.5, h: 0.35, fontFace: NK, fontSize: 11, bold: true, color: CYAN, align: "center", charSpacing: 2, margin: 0 });

// ════════ 6. 05 홀로그램 도감 & 다이제틱 UI ════════
s = p.addSlide(); bg(s); sec(s, "05", "홀로그램 도감 & 다이제틱 UI", "Holographic Codex · Diegetic UI"); foot(s, 6);
const f3 = [["데이터 기반 도감", "이름·수심·스캔시간·모델을 데이터테이블 한 줄로 관리 → 확장 용이"],
["영구 수집 저장", "Subsystem에 기록 → 닫았다 열어도 수집 상태 유지"],
["다이제틱 UI", "손목 수심계·홀로그램 도감 — UI를 화면이 아닌 '세계 안'에 배치"],
["한글 표시", "수집한 종 정보를 홀로그램에 한글 폰트로 출력"]];
f3.forEach((d, i) => {
  const y = 1.95 + i * 1.2; dot(s, 0.8, y + 0.06, GOLD);
  s.addText(d[0], { x: 1.35, y: y, w: 6.1, h: 0.45, fontFace: NK, fontSize: 17, bold: true, color: WHITE, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.45, w: 6.3, h: 0.65, fontFace: NK, fontSize: 13, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
card(s, 8.1, 1.95, 4.5, 4.6, CARD2);
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 8.35, y: 2.2, w: 4.0, h: 1.85, rectRadius: 0.08, fill: { color: "06223B" }, line: { color: CYAN, width: 1 } });
dot(s, 9.95, 2.5, MINT, 0.9);
s.addText("No.07", { x: 8.55, y: 2.4, w: 1.6, h: 0.4, fontFace: BLACK, fontSize: 13, color: GOLD, margin: 0 });
s.addText("Mandarine Fish", { x: 8.55, y: 4.2, w: 3.7, h: 0.4, fontFace: NK, fontSize: 16, bold: true, color: WHITE, margin: 0 });
[["수심", "12 m"], ["스캔시간", "3.0초"], ["수집", "완료 ✓"]].forEach((r, i) => {
  const yy = 4.75 + i * 0.45;
  s.addText(r[0], { x: 8.55, y: yy, w: 1.7, h: 0.36, fontFace: NK, fontSize: 13, color: MUTE, margin: 0 });
  s.addText(r[1], { x: 10.3, y: yy, w: 2.0, h: 0.36, fontFace: NK, fontSize: 13, bold: true, color: i === 2 ? MINT : TXT, margin: 0 });
});

// ════════ 7. 06 살아있는 심해 ════════
s = p.addSlide(); bg(s); sec(s, "06", "살아있는 심해 — 생물 AI & 수심", "Living Deep Sea"); foot(s, 7);
const f4 = [["자연스러운 유영", "전진 + 랜덤 방향 전환으로 살아있는 움직임"], ["예측형 벽 회피", "앞을 미리 감지해 지형에 박히지 않고 부드럽게 회피"], ["수심대 & 도망", "종마다 사는 깊이대 유지, 스캔당하면 빠르게 도망"], ["수심 기반 자동 스폰", "데이터 수심값대로 깊은 종은 더 깊은 곳에 배치"]];
f4.forEach((d, i) => {
  const y = 1.95 + i * 1.2; dot(s, 0.8, y + 0.06, ACCENTS[i]);
  s.addText(d[0], { x: 1.35, y: y, w: 6.0, h: 0.45, fontFace: NK, fontSize: 17, bold: true, color: WHITE, margin: 0 });
  s.addText(d[1], { x: 1.35, y: y + 0.45, w: 6.1, h: 0.6, fontFace: NK, fontSize: 13, color: MUTE, lineSpacingMultiple: 1.1, margin: 0 });
});
const bx = 8.1, bw = 4.5, by = 1.95;
const bands = [["얕은 수심", "0E5C86", ["Bluetang", "Clione"]], ["중간 수심", "0B3F63", ["Mandarine", "Lionfish"]], ["깊은 수심", "061F38", ["Shark", "Whale"]]];
bands.forEach((b, i) => {
  const yy = by + i * 1.5; s.addShape(p.shapes.RECTANGLE, { x: bx, y: yy, w: bw, h: 1.45, fill: { color: b[1] }, line: { color: BORDER, width: 0.5 } });
  s.addText(b[0], { x: bx + 0.25, y: yy + 0.16, w: 2.5, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: CYAN2, margin: 0 });
  s.addText(b[2].join("  ·  "), { x: bx + 0.25, y: yy + 0.64, w: 4.0, h: 0.4, fontFace: NK, fontSize: 12, color: TXT, margin: 0 });
  for (let k = 0; k < 3; k++) dot(s, bx + 3.5 + k * 0.3, yy + 0.95, i === 2 ? PINK : CYAN, 0.16);
});
s.addText("↓ 깊어질수록 다른 종이 산다", { x: bx, y: by + 4.5, w: bw, h: 0.35, fontFace: NK, fontSize: 11, italic: true, color: MUTE, align: "center", margin: 0 });

// ════════ 8. 07 사운드 & 몰입 ════════
s = p.addSlide(); bg(s); sec(s, "07", "사운드 & 몰입 연출", "Audio & Immersion"); foot(s, 8);
const au = [["배경음 자동 정지/재개", "스캔·소나 중 BGM을 멈췄다가 끝나면 멈춘 지점부터 이어 재생"], ["스캔 사운드", "스캔 루프음 + 완료음으로 명확한 피드백"], ["소나 사운드", "가동음 / 감지음 분리로 탐지 단계별 청각 신호"], ["공간감", "안내음은 또렷한 2D, 환경음은 입체적인 3D로 구분"]];
au.forEach((d, i) => {
  const y = 1.95 + i * 1.18; card(s, 0.72, y, 7.3, 1.02);
  s.addShape(p.shapes.OVAL, { x: 1.0, y: y + 0.26, w: 0.5, h: 0.5, fill: { color: ACCENTS[i] }, line: { type: "none" } });
  s.addText(String(i + 1), { x: 1.0, y: y + 0.26, w: 0.5, h: 0.5, fontFace: BLACK, fontSize: 14, color: BG0, align: "center", valign: "middle", margin: 0 });
  s.addText(d[0], { x: 1.7, y: y + 0.13, w: 6.1, h: 0.42, fontFace: NK, fontSize: 16, bold: true, color: WHITE, margin: 0 });
  s.addText(d[1], { x: 1.7, y: y + 0.55, w: 6.2, h: 0.42, fontFace: NK, fontSize: 12, color: MUTE, margin: 0 });
});
card(s, 8.3, 1.95, 4.3, 4.7, CARD2);
const eqx = 8.72, eqB = 5.9, hs = [1.2, 2.3, 1.6, 3.0, 2.0, 2.7, 1.4, 2.4];
hs.forEach((hh, i) => s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: eqx + i * 0.46, y: eqB - hh, w: 0.32, h: hh, rectRadius: 0.05, fill: { color: i % 2 ? CYAN : MINT }, line: { type: "none" } }));
s.addText("몰입을 만드는 소리", { x: 8.3, y: 6.05, w: 4.3, h: 0.4, fontFace: NK, fontSize: 13, bold: true, color: CYAN2, align: "center", margin: 0 });

// ════════ 9. 08 핵심 알고리즘 ════════
s = p.addSlide(); bg(s); sec(s, "08", "핵심 알고리즘", "Key Algorithms"); foot(s, 9);
card(s, 0.72, 1.85, 5.9, 3.65, CARD);
dot(s, 1.0, 2.12, CYAN, 0.55); s.addText("1", { x: 1.0, y: 2.12, w: 0.55, h: 0.55, fontFace: BLACK, fontSize: 16, color: BG0, align: "center", valign: "middle", margin: 0 });
s.addText("퍼지 이름 매칭", { x: 1.7, y: 2.05, w: 4.7, h: 0.45, fontFace: NK, fontSize: 18, bold: true, color: WHITE, margin: 0 });
s.addText("레벤슈타인 편집거리 · 오타/대소문자 보정", { x: 1.7, y: 2.5, w: 4.85, h: 0.35, fontFace: NK, fontSize: 11, color: MUTE, margin: 0 });
s.addText([{ text: "① 완전 일치", options: { breakLine: true, color: TXT } }, { text: "② 정규화 일치", options: { breakLine: true, color: TXT } }, { text: "③ 접두/부분 일치", options: { breakLine: true, color: TXT } }, { text: "④ 편집거리 ≤ 1 (오타 1글자)", options: { color: GOLD, bold: true } }], { x: 1.05, y: 3.0, w: 5.3, h: 1.55, fontFace: NK, fontSize: 13.5, lineSpacingMultiple: 1.3, margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 1.05, y: 4.72, w: 5.3, h: 0.58, rectRadius: 0.1, fill: { color: BG0 }, line: { color: BORDER, width: 1 } });
s.addText([{ text: "\"madarine\" → ", options: { color: MUTE } }, { text: "mandarine", options: { color: GOLD, bold: true } }, { text: "  ✓", options: { color: MINT, bold: true } }], { x: 1.25, y: 4.72, w: 5.0, h: 0.58, fontFace: "Consolas", fontSize: 13.5, valign: "middle", margin: 0 });
card(s, 6.72, 1.85, 5.9, 3.65, CARD);
dot(s, 7.0, 2.12, GOLD, 0.55); s.addText("2", { x: 7.0, y: 2.12, w: 0.55, h: 0.55, fontFace: BLACK, fontSize: 16, color: BG0, align: "center", valign: "middle", margin: 0 });
s.addText("각반경 콘 판정", { x: 7.7, y: 2.05, w: 4.7, h: 0.45, fontFace: NK, fontSize: 18, bold: true, color: WHITE, margin: 0 });
s.addText("벡터 기하 기반 스캔 타겟팅", { x: 7.7, y: 2.5, w: 4.85, h: 0.35, fontFace: NK, fontSize: 11, color: MUTE, margin: 0 });
const apx = 7.35, apy = 3.5;
s.addShape(p.shapes.LINE, { x: apx, y: apy, w: 2.3, h: -0.55, line: { color: GOLD, width: 1.5 } });
s.addShape(p.shapes.LINE, { x: apx, y: apy, w: 2.3, h: 0.55, line: { color: GOLD, width: 1.5 } });
dot(s, apx - 0.12, apy - 0.12, GOLD, 0.24); dot(s, apx + 2.3, apy - 0.18, PINK, 0.36);
s.addText("손전등", { x: apx - 0.35, y: apy + 0.2, w: 1.2, h: 0.3, fontFace: NK, fontSize: 10, color: MUTE, margin: 0 });
s.addText("생물", { x: apx + 2.0, y: apy + 0.2, w: 1.0, h: 0.3, fontFace: NK, fontSize: 10, color: MUTE, align: "center", margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 7.05, y: 4.45, w: 5.25, h: 0.85, rectRadius: 0.1, fill: { color: BG0 }, line: { color: BORDER, width: 1 } });
s.addText([{ text: "θ = acos( Forward · toFish )", options: { breakLine: true, color: CYAN2 } }, { text: "허용각 += asin( r / dist )", options: { color: GOLD } }], { x: 7.25, y: 4.45, w: 5.0, h: 0.85, fontFace: "Consolas", fontSize: 12, valign: "middle", lineSpacingMultiple: 1.15, margin: 0 });
s.addShape(p.shapes.ROUNDED_RECTANGLE, { x: 0.72, y: 5.7, w: 11.9, h: 0.92, rectRadius: 0.1, fill: { color: CARD2 }, line: { color: BORDER, width: 1 } });
dot(s, 1.0, 5.92, MINT, 0.48); s.addText("3", { x: 1.0, y: 5.92, w: 0.48, h: 0.48, fontFace: BLACK, fontSize: 14, color: BG0, align: "center", valign: "middle", margin: 0 });
s.addText([{ text: "생물 AI — 예측형 회피 + 스티어링:  ", options: { bold: true, color: WHITE } }, { text: "앞을 미리 스윕해 충돌 전 법선 방향으로 회피 · 목표 수심 비례제어 · 등속 보간 회전", options: { color: MUTE } }], { x: 1.7, y: 5.82, w: 10.7, h: 0.7, fontFace: NK, fontSize: 13, valign: "middle", margin: 0 });

// ════════ 10. 09 기술 스택 ════════
s = p.addSlide(); bg(s); sec(s, "09", "개발 환경 & 기술 스택", "Tech Stack"); foot(s, 10);
const tech = [["ENGINE", "Unreal Engine 5", ["Lumen 실시간 GI", "Nanite 메시 최적화", "Water · 수중 환경"], CYAN],
["LANGUAGE", "C++ / Blueprint", ["핵심 로직 C++", "연출·이벤트 BP", "모듈형 설계"], MINT],
["DATA / XR", "DataTable · OpenXR", ["도감 데이터 구동", "Subsystem 영구저장", "OpenXR VR 입력"], GOLD]];
tech.forEach((t, i) => {
  const x = 0.72 + i * 4.06; card(s, x, 1.95, 3.82, 4.55);
  s.addShape(p.shapes.RECTANGLE, { x, y: 1.95, w: 0.1, h: 4.55, fill: { color: t[3] }, line: { type: "none" } });
  s.addText(t[0], { x: x + 0.32, y: 2.2, w: 3.3, h: 0.35, fontFace: NK, fontSize: 12, bold: true, color: t[3], charSpacing: 2, margin: 0 });
  s.addText(t[1], { x: x + 0.32, y: 2.58, w: 3.3, h: 0.6, fontFace: NK, fontSize: 19, bold: true, color: WHITE, margin: 0 });
  s.addText(t[2].map(it => ({ text: it, options: { bullet: { code: "2022", indent: 14 }, breakLine: true, color: TXT } })), { x: x + 0.45, y: 3.35, w: 3.15, h: 2.9, fontFace: NK, fontSize: 13.5, lineSpacingMultiple: 1.35, valign: "top", margin: 0 });
});

// ════════ 11. 10 콘텐츠 ════════
s = p.addSlide(); bg(s); sec(s, "10", "콘텐츠 — 해양 생물 & 환경", "Content"); foot(s, 11);
s.addText("해양 생물 12종과 광활한 심해 환경을 제작했습니다.", { x: 0.72, y: 1.62, w: 12, h: 0.4, fontFace: NK, fontSize: 14, color: MUTE, margin: 0 });
const fish = ["Blobfish", "Bluetang", "Clione", "Crownfish", "Lionfish", "Lure", "Mandarine", "Seahorse", "Shark", "Turtle", "Whale", "Octopus"];
const gx = 0.72, gy = 2.25, gw = 2.95, gh = 0.78, ggx = 0.13, ggy = 0.13;
fish.forEach((f, i) => {
  const col = i % 4, row = Math.floor(i / 4), x = gx + col * (gw + ggx), y = gy + row * (gh + ggy);
  s.addShape(p.shapes.ROUNDED_RECTANGLE, { x, y, w: gw, h: gh, rectRadius: 0.08, fill: { color: CARD }, line: { color: BORDER, width: 1 } });
  s.addShape(p.shapes.OVAL, { x: x + 0.18, y: y + 0.19, w: 0.4, h: 0.4, fill: { color: ACCENTS[i % ACCENTS.length] }, line: { type: "none" } });
  s.addText(String(i + 1), { x: x + 0.18, y: y + 0.19, w: 0.4, h: 0.4, fontFace: BLACK, fontSize: 12, color: BG0, align: "center", valign: "middle", margin: 0 });
  s.addText(f, { x: x + 0.72, y: y, w: gw - 0.8, h: gh, fontFace: NK, fontSize: 14, bold: true, color: WHITE, valign: "middle", margin: 0 });
});
card(s, 0.72, 5.6, 11.9, 1.05, CARD2);
s.addText("심해 환경", { x: 1.05, y: 5.75, w: 2, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: GOLD, margin: 0 });
s.addText("산호초 군락 · 해초 숲 · 침몰한 고대 유적 · 거대 고래뼈 · 심해 기지(DeepWaterStation)", { x: 1.05, y: 6.15, w: 11.3, h: 0.4, fontFace: NK, fontSize: 13.5, color: TXT, margin: 0 });

// ════════ 12. 11 중간 → 기말 완성도 ════════
s = p.addSlide(); bg(s); sec(s, "11", "중간 → 기말, 완성한 것", "Final Build Status"); foot(s, 12);
s.addText("중간발표의 '기반 구조'를 실제 동작하는 '완성된 시연 흐름'으로 연결했습니다.", { x: 0.72, y: 1.62, w: 12, h: 0.4, fontFace: NK, fontSize: 14, color: MUTE, margin: 0 });
const done = ["스캔 락온 + 생물별 스캔 시간", "소나 → 발견 → 스캔 → 도감 연결", "도감 영구 저장(Subsystem)", "손목 수심계(다이제틱 UI)", "스캔/소나 효과음 + BGM 자동 덕킹", "생물 AI: 유영·회피·도망", "수심 기반 자동 스폰", "해양 생물 12종 + 심해 환경"];
done.forEach((d, i) => {
  const col = i % 2, row = Math.floor(i / 2), x = 0.72 + col * 6.06, y = 2.25 + row * 1.08;
  card(s, x, y, 5.85, 0.92);
  s.addText("✓", { x: x + 0.25, y: y, w: 0.6, h: 0.92, fontFace: NK, fontSize: 22, bold: true, color: MINT, align: "center", valign: "middle", margin: 0 });
  s.addText(d, { x: x + 0.95, y: y, w: 4.7, h: 0.92, fontFace: NK, fontSize: 14.5, bold: true, color: WHITE, valign: "middle", margin: 0 });
});

// ════════ 13. 시연 영상 ════════
s = p.addSlide(); bg(s);
s.addShape(p.shapes.OVAL, { x: W / 2 - 0.9, y: 2.0, w: 1.8, h: 1.8, fill: { color: CYAN }, line: { type: "none" }, shadow: sh() });
s.addText("▶", { x: W / 2 - 0.72, y: 2.0, w: 1.8, h: 1.8, fontFace: NK, fontSize: 42, color: BG0, align: "center", valign: "middle", margin: 0 });
s.addText("DEMO", { x: 0, y: 3.95, w: W, h: 0.4, fontFace: NK, fontSize: 14, bold: true, color: MINT, align: "center", charSpacing: 5, margin: 0 });
s.addText("시연 영상", { x: 0, y: 4.35, w: W, h: 0.8, fontFace: BLACK, fontSize: 44, color: WHITE, align: "center", margin: 0 });
s.addText("약 2분  ·  탐사 → 소나 발견 → 스캔 → 도감 등록의 실제 플레이", { x: 0, y: 5.3, w: W, h: 0.5, fontFace: NK, fontSize: 17, color: TXT, align: "center", margin: 0 });

// ════════ 14. 마무리 ════════
s = p.addSlide(); bg(s);
s.addText("THANK YOU", { x: 0.9, y: 1.5, w: 11, h: 0.5, fontFace: NK, fontSize: 14, bold: true, color: CYAN, charSpacing: 5, margin: 0 });
s.addText("심해의 발견을 도감에 담다", { x: 0.84, y: 1.98, w: 12, h: 1.0, fontFace: BLACK, fontSize: 38, color: WHITE, margin: 0 });
s.addText("핵심 요약", { x: 0.95, y: 3.4, w: 5.5, h: 0.4, fontFace: NK, fontSize: 16, bold: true, color: GOLD, margin: 0 });
s.addText([{ text: "VR 심해 탐사 + 소나·스캔으로 홀로그램 도감 수집", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "데이터 기반 설계 · 영구 저장으로 확장성 확보", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "반응하는 생물 AI로 살아있는 심해 구현", options: { bullet: { code: "2022" } } }], { x: 0.95, y: 3.85, w: 5.7, h: 2.2, fontFace: NK, fontSize: 14, color: TXT, lineSpacingMultiple: 1.4, margin: 0 });
s.addText("향후 계획", { x: 7.0, y: 3.4, w: 5.5, h: 0.4, fontFace: NK, fontSize: 16, bold: true, color: GOLD, margin: 0 });
s.addText([{ text: "해양 생물 종 추가 및 희귀종 도입", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "미션/퀘스트 · 사진 모드 등 콘텐츠 확장", options: { bullet: { code: "2022" }, breakLine: true } }, { text: "사운드·비주얼 폴리싱으로 몰입 강화", options: { bullet: { code: "2022" } } }], { x: 7.0, y: 3.85, w: 5.6, h: 2.2, fontFace: NK, fontSize: 14, color: TXT, lineSpacingMultiple: 1.4, margin: 0 });
s.addText("DEEP DIVE VR  ·  Unreal Engine 5  ·  OpenXR", { x: 0, y: 6.7, w: W, h: 0.4, fontFace: NK, fontSize: 11, color: DIM, align: "center", margin: 0 });

p.writeFile({ fileName: "C:/Users/pch31/Documents/GitHub/Seaproject/발표자료/DeepDiveVR_기말발표.pptx" }).then(f => console.log("SAVED:", f));
