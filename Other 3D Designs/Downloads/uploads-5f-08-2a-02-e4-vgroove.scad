// v groove bearing cover
// rrhb creative commons 6/18/2013

// flare > 1 exaggerates angle
// bearingd = bearing diameter
// hole = bearing outer diameter
// thickness = bearing thickness
// tolerance = how much slop to allow for pressfit bearing
// hole defaults to bearing size or can be the size of the diam of hole

module vwheel(bearingd=13,thickness=5,flare=1.2,tolerance=0.05,hole=0) {
  base = bearingd/4;  // thinnest portion of vgroove to bearing
  outer = (bearingd/2+base)*flare;
  inner = bearingd/2+base;
  hole = (hole==0 ? bearingd : hole);

  difference() {
    union() {
      cylinder(r1=outer,r2=inner,h=thickness/2);
      translate([0,0,thickness/2]) cylinder(r2=outer,r1=inner,h=thickness/2);
    }
    cylinder(r=hole/2+tolerance,h=thickness*2);
  }
}

// vwheel(bearingd=10,thickness=4,hole=5) ;   // 623-sized hole
//vwheel(bearingd=10,thickness=4) ;   // 623
//vwheel(bearingd=13,thickness=5) ; // 624
//vwheel(bearingd=16,thickness=5) ; // 625
vwheel(bearingd=24,thickness=8) ; // 628

