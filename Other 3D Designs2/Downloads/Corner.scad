difference(){
union(){
for(x=[0:1]){
			rotate([0,0,x*90])translate([0,32.5,32.5])cube([65,5,35], center=true);	
}translate([-32.5, 32.5,32.5])cylinder(r=2.5,h=35, center=true);

}
union()
{
			translate([1,32.5,34.5])cube([65,2.5,33], center=true);
			translate([1,30,49])cube([65,4,2.5], center=true);
			rotate([0,0,90])translate([1,32.5,34.5])cube([65,2.5,33], center=true);
			rotate([0,0,90])translate([1,30,49])cube([65,4, 2.5], center=true);
			translate([30,-32.5,34.5])cube([65,7,50], center=true);

			translate([30,32.5,34.5])cube([90,7,50], center=true);
			translate([-32.5,-30,34.5])cube([7,90,50], center=true);
}
}