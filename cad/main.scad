PLY_THICKNESS = 3;

GRID_SIZE = 1000 / 60 * 2;  // 2 LEDs per grid, 60 LEDS per metre
GRID_THICKNESS = 5;
GRID_COUNT_X = 7;
GRID_COUNT_Y = 9;

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

BAFFLE_HEIGHT = 3 * PLY_THICKNESS;


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
      for (i = [-3.5, -2.5, -0.5, 0.5, 2.5, 3.5]) {
        grid_horizontal(i);
      }
    }
    
    // Cutout for text grid
    cube([
      TEXTGRID_X * GRID_SIZE - GRID_THICKNESS, 
      TEXTGRID_Y * GRID_SIZE - GRID_THICKNESS, 
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

module baffle(length) {
  difference() {
    square([length, BAFFLE_HEIGHT], center = true);
  }
}

module baffle_vertical(pos) {
  
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

translate([0, 0, PLY_THICKNESS *  0]) layer1();
translate([0, 0, PLY_THICKNESS * -1]) layer2();
translate([0, 0, PLY_THICKNESS * -2]) layer3();
