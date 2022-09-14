///////////////
// Constants //
///////////////

$fs=0.5;  // mm
$fa=5;    // degrees

PLY_THICKNESS = 3;

GRID_SIZE = 1000 / 60 * 2;  // 2 LEDs per grid, 60 LEDS per metre
GRID_THICKNESS = 5;
GRID_COUNT_X = 7;
GRID_COUNT_Y = 9;

FRAME_WIDTH = 20;

INNER_WIDTH = GRID_COUNT_X * GRID_SIZE - GRID_THICKNESS;
INNER_HEIGHT = GRID_COUNT_Y * GRID_SIZE - GRID_THICKNESS;
OUTER_WIDTH = INNER_WIDTH + 2 * FRAME_WIDTH;
OUTER_HEIGHT = INNER_HEIGHT + 2 * FRAME_WIDTH;
echo(OUTER_WIDTH);
echo(OUTER_HEIGHT);

BORDER_SPACING = GRID_SIZE * 0.8;
BORDER_OVERHANG = 10;
BORDER_SHORT_LENGTH = BORDER_SPACING * 2 + BORDER_OVERHANG;

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

LED_WIDTH = 5.5;
LED_HEIGHT = 5.5;
LED_SPACING = 1000 / 60;

CUTOUT_HEIGHT = 40;
CUTOUT_WIDTH = 160;



////////////////
// Components //
////////////////

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

module grid2_piece(length) {
  translate([0, 0, PLY_THICKNESS / 2]) {
    cube([length, GRID_THICKNESS, PLY_THICKNESS], center = true);
  }
}

module grid2_corner() {
  translate([0, INNER_HEIGHT/2 - BORDER_SPACING, 0])
  grid2_piece(INNER_WIDTH);
  
  translate([-INNER_WIDTH/2 + BORDER_SPACING, 0, 0])
  rotate([0, 0, 90])
  grid2_piece(INNER_HEIGHT);
  
  translate([-(INNER_WIDTH - BORDER_SHORT_LENGTH) / 2, INNER_HEIGHT/2 - BORDER_SPACING * 2, 0])
  grid2_piece(BORDER_SHORT_LENGTH);
  
  translate([-INNER_WIDTH/2 + BORDER_SPACING * 2, (INNER_HEIGHT - BORDER_SHORT_LENGTH) / 2, 0])
  rotate([0, 0, 90])
  grid2_piece(BORDER_SHORT_LENGTH);
}

module grid2() {
  union() {
    grid2_corner();
    mirror([1, 0, 0]) grid2_corner();
    mirror([0, 1, 0]) {
      grid2_corner();
      mirror([1, 0, 0]) grid2_corner();
    }
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

module led_mount_screw_holes(n) {
  length = n * GRID_SIZE;
  for (i = [-length/2 : GRID_SIZE : length/2]) {
    translate([0, i, 0])
    circle(d = 3);
  }
}

module led_cutouts(n) {
  length = (n * 2 - 1) * LED_SPACING;
  for (i = [-length/2 : LED_SPACING : length/2]) {
    translate([i, 0, 0])
    square([LED_WIDTH, LED_HEIGHT], center = true);
  }
}

module led_mount() {
  linear_extrude(PLY_THICKNESS) {
    difference() {
      square([TEXTGRID_WIDTH + 25, TEXTGRID_HEIGHT + 10], center = true);
      
      for (i = [0 : TEXTGRID_Y - 1]) {
        translate([
          0,
          (i - (TEXTGRID_X - 1) / 2) * -GRID_SIZE
        ])
        led_cutouts(TEXTGRID_X);
      }
      
      translate([TEXTGRID_WIDTH / 2 + 7, 0, 0])
      led_mount_screw_holes(TEXTGRID_Y);
      
      translate([-TEXTGRID_WIDTH / 2 - 7, 0, 0])
      led_mount_screw_holes(TEXTGRID_Y);
    }
  }
}

module screwhole_cutouts() {
  translate([INNER_WIDTH/2 + 6, TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  circle(d = 3);
  
  translate([INNER_WIDTH/2 + 6, -TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  circle(d = 3);
  
  translate([-INNER_WIDTH/2 - 6, TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  circle(d = 3);
  
  translate([-INNER_WIDTH/2 - 6, -TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  circle(d = 3);
}

module nuthole_cutout() {
  circle(r = 3.1, $fn = 6);
}

module nuthole_cutouts() {
  translate([INNER_WIDTH/2 + 6, TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  nuthole_cutout();
  
  translate([INNER_WIDTH/2 + 6, -TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  nuthole_cutout();
  
  translate([-INNER_WIDTH/2 - 6, TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  nuthole_cutout();
  
  translate([-INNER_WIDTH/2 - 6, -TEXTGRID_HEIGHT/2, -1])
  linear_extrude(PLY_THICKNESS * 2)
  nuthole_cutout();
}



////////////
// Layers //
////////////

module layer1() {
  color([0.5, 0, 0]) frame();
  //color([0.4, 0.2, 0]) grid();
  color([0.4, 0.2, 0]) grid2();
  color([0.4, 0.2, 0]) grid2();
}

module layer2() {
  color([0.6, 0.2, 0.2]) frame(half = true);
  color([1, 0.8, 0.5]) linear_extrude(PLY_THICKNESS) difference() {
    square([INNER_WIDTH + FRAME_WIDTH, INNER_HEIGHT + FRAME_WIDTH], center = true);
    textgrid();
  }
}

module framelayer(screwholes = false, nutholes = false) {
  color([0.8, 0.5, 0.5]) difference() {    
    frame();
    
    if (screwholes) {screwhole_cutouts();}
    if (nutholes) {nuthole_cutouts();}
  }
}

module backframe() {
  linear_extrude(PLY_THICKNESS)
  difference() {
    square([OUTER_WIDTH, OUTER_HEIGHT], center = true);
    square([OUTER_WIDTH-FRAME_WIDTH, OUTER_HEIGHT-FRAME_WIDTH], center = true);
  }
}

module backboard() {
  color([0.8, 1, 0.8])
  difference() {
    linear_extrude(PLY_THICKNESS) {
      difference() {
        square([OUTER_WIDTH-FRAME_WIDTH-1, OUTER_HEIGHT-FRAME_WIDTH-1], center = true);
        
        // Bottom cutout
        cut_pos = -INNER_HEIGHT/2 + CUTOUT_HEIGHT/2;
        translate([0, cut_pos, 0]) square([CUTOUT_WIDTH, CUTOUT_HEIGHT], center = true);
      }
    }
    
    screwhole_cutouts();
  }
}



//////////////
// Assembly //
//////////////

//projection() {    // Uncomment for export
translate([0, 0, PLY_THICKNESS *  1]) layer1();
translate([0, 0, PLY_THICKNESS *  0]) layer1();
translate([0, 0, PLY_THICKNESS * -1]) layer2();
translate([0, 0, PLY_THICKNESS * -1]) baffles();
translate([0, 0, PLY_THICKNESS * -2]) framelayer(screwholes = true);
translate([0, 0, PLY_THICKNESS * -3]) framelayer(screwholes = true);
translate([0, 0, PLY_THICKNESS * -4]) framelayer(screwholes = true);
translate([0, 0, PLY_THICKNESS * -5]) framelayer(screwholes = true);
translate([0, 0, PLY_THICKNESS * -6]) framelayer(nutholes = true);
translate([0, 0, PLY_THICKNESS * -6]) led_mount();
translate([0, 0, PLY_THICKNESS * -7]) framelayer(screwholes = true);
translate([0, 0, PLY_THICKNESS * -8]) backframe();
translate([0, 0, PLY_THICKNESS * -8]) backboard();
//rotate([0, 90, 0]) baffle_vertical(1);      // For export only
//rotate([90, 0, 0]) baffle_horizontal(1);    // For export only
//}                 // Uncomment for export