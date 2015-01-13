include <beagleboneblack.scad>

wallThickness = 1.5;
wallSpace = 1.0;  // REVISIT - for ABS printing this needs to be larger.
floorThickness = 1.0;
floorSpace = 5.0;

bbbPosition = [wallThickness+wallSpace,wallThickness+wallSpace,floorThickness+floorSpace];
boxHeight = floorThickness+floorSpace+ethSize[2]+ethOffset[2]-small;
boxSize = [boardLength+2*wallThickness+2*wallSpace, boardWidth+2*wallThickness+2*wallSpace, boxHeight];

inch = 25.4;
r1 = 0.250 * inch;
r2 = 0.500 * inch;

standoffHeight = floorSpace;
standoffR1 = 3.9/2;
standoffR2 = holeR * 0.95;
standoffBaseR = 7.0;
standoffBaseHeight = standoffHeight-3.0;

extraPowerSize = [14.0,9.5,14.3];  // expand opening to top

// unused
standoffBraceSize = [3,standoffR1,standoffHeight];

lidThickness = 1.0;
lidLipHeight = 5.0;
lidLipThickness = 0.7;

module boxCornerNegative(centre, size, rotation)
{
  translate(centre) {
    rotate([0,0,rotation])
    {
      difference()
      {
        translate([-small,-small,-small/2]) {
          cube([size[0]+small,size[1]+small,size[2]+small]);
        }
        translate([size[0],size[1],-small]) {
          cylinder(h=size[2]+small*2,r=size[0], center=false);
        }
      }
    }
  }
}


// unused
/*      
 *   Y  /\
 *     |\ \
 *     | \ \
 *   Z |  \ \
 *     |   \/
 *     +----
 *       X
 */
module brace(size)
{
  polyhedron (
    points = [
      [0, 0, size[2]],
      [0, size[1], size[2]], 
      [0, size[1], 0], 
      [0, 0, 0],
      [size[0], 0, 0],
      [size[0], size[1], 0]
    ],
    triangles = [[0,3,2],
                 [0,2,1],
                 [3,0,4],
                 [1,2,5], 
                 [0,5,4],
                 [0,1,5],
                 [5,2,4],
                 [4,2,3],
   ]
  );
}

// unused
module standoffBrace(position, angle)
{
  rotate([0,0,angle]) {
    translate([0,-standoffBraceSize[1]/2,0]) {
      brace(standoffBraceSize);
    }
  }
}

module standoff(position)
{
  translate(position)
  {
    cylinder(h=standoffBaseHeight, r=standoffBaseR);
    cylinder(h=standoffHeight, r=standoffR1);
    cylinder(h=standoffHeight+boardHeight*1.5, r=standoffR2);
/*
    standoffBrace(position,0);
    standoffBrace(position,90);
    standoffBrace(position,180);
    standoffBrace(position,270);
*/
  }
}

module boxPositive()
{
  difference()
  {
    cube(boxSize);
    boxCornerNegative([0,0,0], [r1, r1, boxSize[2]], 0);
    boxCornerNegative([0,boxSize[1],0], [r1, r1, boxSize[2]], 270);
    boxCornerNegative([boxSize[0],0,0], [r2, r2, boxSize[2]], 90);
    boxCornerNegative([boxSize[0],boxSize[1],0], [r2, r2, boxSize[2]], 180);
  }
}

module boxNegative()
{
  innerSize = [boardLength+2*wallSpace,boardWidth+2*wallSpace,boxHeight-floorThickness+small];
  translate([wallThickness,wallThickness,floorThickness]) 
  {
    difference()
    {
      cube(innerSize);
      boxCornerNegative([0,0,0], [r1, r1, boxHeight], 0);
      boxCornerNegative([0,innerSize[1],0], [r1, r1, boxHeight], 270);
      boxCornerNegative([innerSize[0],0,0], [r2, r2, boxHeight], 90);
      boxCornerNegative([innerSize[0],innerSize[1],0], [r2, r2, boxHeight], 180);
    }
  }
}

module standoffs()
{
  translate([bbbPosition[0],bbbPosition[1],floorThickness]) {
    standoff(hole1Pos);
    standoff(hole2Pos);
    standoff(hole3Pos);
    standoff(hole4Pos);
  }
}

module supports()
{
  supportWidth = 1.0;
  translate([boxSize[0]-wallThickness,18.0,0]) cube([wallThickness,supportWidth,boxHeight]);
  translate([boxSize[0]-wallThickness,38.0,0]) cube([wallThickness,supportWidth,boxHeight]);
}

module roundedBox(size, radii)
{
  difference()
  {
    cube(size);
    boxCornerNegative([0,0,0],[radii[0],radii[0],size[2]],0);
    boxCornerNegative([0,size[1],0],[radii[1],radii[1],size[2]],270);
    boxCornerNegative([size[0],0,0],[radii[2],radii[2],size[2]],90);
    boxCornerNegative([size[0],size[1],0],[radii[3],radii[3],size[2]],180);
  }
}

module lid()
{
  translate([0,0,boxHeight]) 
  {
    roundedBox([boxSize[0],boxSize[1],lidThickness],[r1,r1,r2,r2]);
  }

  tolerance = 0.6;
  lipSize = [boxSize[0]-wallThickness*2-tolerance*2, boxSize[1]-wallThickness*2-tolerance*2, lidLipHeight];
  lipNegSize = [lipSize[0]-lidLipThickness*2,lipSize[1]-lidLipThickness*2,lipSize[2]+small];
  lipR1 = r1-wallThickness-tolerance;
  lipR2 = r2-wallThickness-tolerance;

  difference() 
  {
    translate([wallThickness+tolerance,wallThickness+tolerance,boxHeight-lidLipHeight])
    {
      roundedBox(lipSize, [lipR1,lipR1,lipR2,lipR2]);
    } 
    translate([wallThickness+tolerance+lidLipThickness,wallThickness+tolerance+lidLipThickness,boxHeight-lidLipHeight-small])
    {
      roundedBox([lipSize[0]-lidLipThickness*2, lipSize[1]-lidLipThickness*2, lipSize[2]+small], [lipR1,lipR1,lipR2,lipR2]);
    }

    translate(bbbPosition) 
    { 
      beagleboneblack();
    }

  }
  
}

module box()
{

  difference() 
  {
    boxPositive();
    boxNegative();
    translate(bbbPosition)
    {
      beagleboneblack();
      translate(powerOffset) cube(extraPowerSize);
    }
  }

  intersection()
  {
    boxNegative();
    standoffs();
  }
  supports();
}

box();

// show the beaglebone
translate(bbbPosition)
{
  //%beagleboneblack();
}

// lid positioned inline-for-assembly
//translate([0,0,10]) lid();

// lid positioned for printing
translate([0,-10,boxSize[2]+lidThickness])
  rotate([180,0,0])
    lid();
