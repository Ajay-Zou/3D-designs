difference() {
	cube([1,.75,.5]);
	translate([.5394,.4616,0])
		cylinder(h = .5, r1 = .0495, r2 = .0495, center = false);
	translate([.5394,.375-.0866,0])
		cylinder(h = .5, r1 = .0495, r2 = .0495, center = false);
	translate([.5-.1417,.4774,0])
		cylinder(h = .5, r1 = .012, r2 = .012, center = false);
	translate([.5-.0945,.375+.1181,0])
		cylinder(h = .5, r1 = .012, r2 = .012, center = false);
}