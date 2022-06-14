PLY_THICKNESS = 3;

GRID_SIZE = 1000 / 60 * 2;  // 2 LEDs per grid, 60 LEDS per metre
GRID_THICKNESS = 5;
GRID_COUNT_X = 7;
GRID_COUNT_Y = 7;

FRAME_WIDTH = 15;

INNER_WIDTH = GRID_COUNT_X * GRID_SIZE - GRID_THICKNESS;
INNER_HEIGHT = GRID_COUNT_Y * GRID_SIZE - GRID_THICKNESS;
OUTER_WIDTH = INNER_WIDTH + 2 * FRAME_WIDTH;
OUTER_HEIGHT = INNER_HEIGHT + 2 * FRAME_WIDTH;
echo(OUTER_WIDTH);
echo(OUTER_HEIGHT);

TEXTGRID = [
["열", "한", "다", "세", "네"],
["두", "여", "섯", "일", "곱"],
["여", "덟", "아", "홉", "시"],
["자", "정", "이", "삼", "십"],
["사", "오", "십", "오", "분"]
];
TEXTGRID_X = len(TEXTGRID);
TEXTGRID_Y = len(TEXTGRID[0]);
TEXTGRID_WIDTH = TEXTGRID_X * GRID_SIZE;
TEXTGRID_HEIGHT = TEXTGRID_X * GRID_SIZE;

BAFFLE_HEIGHT = 4 * PLY_THICKNESS;


module frame_piece(length, width, half = false) {
  linear_extrude(PLY_THICKNESS) {
    difference() {
      // Main piece
      square([length, width], center = true);
      
      // Chamfer 1
      translate([-length/2, -width/2]) rotate(45) square(width * 2);
      
      // Chamfer 2
      translate([length/2, -width/2]) rotate(45) square(width * 2);
      
      // Half size
      if (half) {
        translate([0, width/2]) square([length, width], center = true);
      }
    }
  }
}

module frame(half = false) {
  trans_x = OUTER_WIDTH/2 - FRAME_WIDTH/2;
  trans_y = OUTER_HEIGHT/2 - FRAME_WIDTH/2;
  translate([-trans_x, 0, 0]) {
    rotate([0, 0, -90]) frame_piece(OUTER_HEIGHT, FRAME_WIDTH, half);
  }
  translate([trans_x, 0, 0]) {
    rotate([0, 0, 90]) frame_piece(OUTER_HEIGHT, FRAME_WIDTH, half);
  }
  translate([0, -trans_y, 0]) {
    rotate([0, 0, 0]) frame_piece(OUTER_WIDTH, FRAME_WIDTH, half);
  }
  translate([0, trans_y, 0]) {
    rotate([0, 0, 180]) frame_piece(OUTER_WIDTH, FRAME_WIDTH, half);
  }
}

module grid_vertical(pos) {
  translate([pos * GRID_SIZE, 0, PLY_THICKNESS / 2]) {
    cube([GRID_THICKNESS, INNER_HEIGHT, PLY_THICKNESS], center = true);
  }
}

module grid_horizontal(pos) {
  translate([0, pos * GRID_SIZE, PLY_THICKNESS / 2]) {
    cube([INNER_WIDTH, GRID_THICKNESS, PLY_THICKNESS], center = true);
  }
}

module grid() {
  difference() {
    // Grid
    union() {
      for (i = [-2.5, -1.5, -0.5, 0.5, 1.5, 2.5]) {
        grid_vertical(i);
      }
      for (i = [-2.5, -0.5, 0.5, 2.5]) {
        grid_horizontal(i);
      }
    }
    
    // Cutout for text grid
    cube([
      TEXTGRID_WIDTH - GRID_THICKNESS, 
      TEXTGRID_HEIGHT - GRID_THICKNESS, 
      PLY_THICKNESS * 3
    ], center = true);
  }
}

module textgrid_cell(s, x, y) {
  translate([
    (y - (TEXTGRID_Y - 1) / 2) * GRID_SIZE,
    (x - (TEXTGRID_X - 1) / 2) * -GRID_SIZE,
    0
  ]) {
    text(
      s, 
      size = 18,
      font = "Noto Serif KR:style=Bold", 
      halign = "center", 
      valign = "center"
    );
  }
}

module textgrid() {
  for (i = [0 : TEXTGRID_X - 1]) {
    for (j = [0 : TEXTGRID_Y - 1]) {
      textgrid_cell(TEXTGRID[i][j], i, j);
    }
  }
}

module diffuser_cell(x, y) {
  translate([
    (y - (TEXTGRID_Y - 1) / 2) * GRID_SIZE,
    (x - (TEXTGRID_X - 1) / 2) * -GRID_SIZE,
    0
  ]) {
    square([GRID_SIZE - PLY_THICKNESS, GRID_SIZE - PLY_THICKNESS], center = true);
  }
}

module m3cutout(length) {
  translate([length / 2, 0, 0])
    square([length, 3], center = true);
  translate([3, 0, 0])
    square([2.5, 5.5], center = true);
}

module baffle(length, margin, joints = false) {
  linear_extrude(PLY_THICKNESS, center = true) {
    difference() {
      // Main body
      square([length + margin, BAFFLE_HEIGHT], center = true);
      
      // Slots
      for (x = [-length/2:GRID_SIZE:length/2]) {
        translate([x, BAFFLE_HEIGHT/2]) {
          square([PLY_THICKNESS, BAFFLE_HEIGHT], center = true);
        }
      }
      
      // Screw holes
      if (joints) {
        translate([-(length/2+7), BAFFLE_HEIGHT/2]) rotate(-90) m3cutout(8);
        translate([(length/2+7), BAFFLE_HEIGHT/2]) rotate(-90) m3cutout(8);
      }
    }
  }
}

module baffle_horizontal(pos) {
  translate([0, pos, -BAFFLE_HEIGHT/2]) {
    rotate([-90, 0, 0]) baffle(TEXTGRID_WIDTH, 25, joints = true);
  }
}

module baffle_vertical(pos) {
  translate([pos, 0, -BAFFLE_HEIGHT/2]) {
    rotate([90, 0, 90]) baffle(TEXTGRID_HEIGHT, 10);
  }
}

module baffles() {
  xrange = [-TEXTGRID_WIDTH/2:GRID_SIZE:TEXTGRID_WIDTH/2];
  yrange = [-TEXTGRID_WIDTH/2:GRID_SIZE:TEXTGRID_WIDTH/2];
  for (pos = xrange) {
    color([1, 0.5, 1]) baffle_vertical(pos);
  }
  for (pos = yrange) {
    color([0.5, 0.5, 1]) baffle_horizontal(pos);
  }
}

module layer1() {
  color([0.5, 0, 0]) frame();
  color([0.4, 0.2, 0]) grid();
}

module layer2() {
  color([0.6, 0.2, 0.2]) frame(half = true);
  color([1, 0.8, 0.5]) linear_extrude(PLY_THICKNESS) difference() {
    square([INNER_WIDTH + FRAME_WIDTH, INNER_HEIGHT + FRAME_WIDTH], center = true);
    textgrid();
  }
}

module layer3() {
  color([0.7, 0.4, 0.4]) frame();
  color([0.9, 0.9, 0.9]) for (i = [0 : TEXTGRID_X - 1]) {
    for (j = [0 : TEXTGRID_Y - 1]) {
      linear_extrude(PLY_THICKNESS) diffuser_cell(i, j);
    }
  }
}

module framelayer() {
  color([0.8, 0.5, 0.5]) frame();
}

module backboard() {
  color([0.8, 1, 0.8]) linear_extrude(PLY_THICKNESS) {
    difference() {
      square([OUTER_WIDTH, OUTER_HEIGHT], center = true);
      
      // Bottom cutout
      cut_pos = -INNER_HEIGHT/2 + GRID_SIZE/2;
      translate([0, cut_pos, 0]) square([TEXTGRID_WIDTH, GRID_SIZE], center = true);
    }
  }
}

translate([0, 0, PLY_THICKNESS *  0]) layer1();
translate([0, 0, PLY_THICKNESS * -1]) layer2();
translate([0, 0, PLY_THICKNESS * -1]) baffles();
translate([0, 0, PLY_THICKNESS * -2]) layer3();
translate([0, 0, PLY_THICKNESS * -3]) framelayer();
translate([0, 0, PLY_THICKNESS * -4]) framelayer();
translate([0, 0, PLY_THICKNESS * -5]) framelayer();
translate([0, 0, PLY_THICKNESS * -6]) framelayer();
translate([0, 0, PLY_THICKNESS * -7]) framelayer();
translate([0, 0, PLY_THICKNESS * -8]) backboard();

