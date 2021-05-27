// logo.scad - Basic example of module, top-level variable and $fn usage

//loadcell(50);
//column(50);
main(50);


module main(size=50, $fn=100) {

    difference(){
        difference(){
            union(){
                translate([-15,7,-7.5])loadcell(50);
                translate([0,-10,0])column();
                translate([0,5,-3])cube([17,10,15], center = true);
                translate([0,0,18]) gopromount();
            }
            translate([0,-7,0]) rotate([90,0,0]) cylinder(d=5,h=5, center = true );

            translate([0,0,-8])cube([50,100,5], center = true);
            cylinder(d=10.5,h=30, center = true);
            translate([0,29,-5.3]) rotate([0,90,0]) cylinder(d=20,h=30, center = true);
            translate([0,0,13]) rotate([0,90,0]) cylinder(d=17,h=30, center = true);
            //#translate([0,5,-4]) cube([50,50,20],center = true);
            
            
        }
        #translate([-7, 10, -2]) cube([14, 47.25, 6.5+0.5]);
    }
}

module gopromount(){
    translate([-4, 20, 0]){ 
        rotate([90, 90, 0]) mount2();
        translate([4,-6,-12])cube([17,28,3], center = true);
        translate([-3.75,-1,-17.5]) cube([1.5,17,12], center = true);
        translate([11.75,-1,-17.5]) cube([1.5,17,12], center = true);
    }
}

    
module column(size=50, $fn=100) {
    intersection(){
        translate([0,7,-0.5]) color("red",1.0) cube([17,20,10], center = true );
        translate([0,10,-15]) rotate([0,0,90]) cylinder(d=17,h=30);
    }
}
    
module loadcell(size=50, $fn=100) {

    column_d = 10;
    column_offset = 0;
    column_width = 0;
    y_axel = 0 + column_width;
    width = 12;
    height = 12;
    topheight = 5;
    x=0;
    y=0+column_width;
    z=0;
    lc_offset_y = 5 + y_axel;
    lc_offset_z = 6;
    lc_hole_y = 2.5;
    lc_x = 10;
    lc_y = 47.25;
    lc_z = 6.5+1;
    second_whole_offset = 6;
    // One positive object (sphere) and three negative objects (cylinders)
    difference() {
        //本体
        translate([x,y,z])cube([30, width, height]);       
        
        //loadcell
        translate([10, lc_offset_y -3.5, lc_offset_z]) cube([lc_x, lc_y, lc_z]);
        
        //loadcell 4mm孔
        translate([12, lc_offset_y + lc_hole_y, -3])rotate([0,0,90]) cylinder(h=15,d=4);
        //loadcell 5.5mm螺丝孔
        translate([12,lc_offset_y + lc_hole_y, -2])rotate([0,0,90]) cylinder(h=5,d=5.5);        
        
        
        
        translate([12 + second_whole_offset, lc_offset_y + lc_hole_y, -3])rotate([0,0,90]) cylinder(h=15,d=4);
        translate([12 + second_whole_offset,lc_offset_y + lc_hole_y, -2])rotate([0,0,90]) cylinder(h=5,d=5.5);
        
        
        translate([-3.5,-1,-4]) cube([10,50,20]);
        translate([23.5,-1,-4]) cube([10,50,20]);
        //线槽
        translate([12, lc_offset_y -3.5, lc_offset_z]) cube([lc_x, lc_y, lc_z]);
        translate([8, lc_offset_y-2, lc_offset_z]) cube([lc_x, lc_y, lc_z]);
        //#translate([0, lc_offset_y-2, lc_offset_z]) cube([lc_x, lc_y, lc_z]);

    }
}

$fa = 2;
$fs = 0.25;
Extra_Mount_Depth = 3;

module nut_hole()
{
	rotate([0, 90, 0]) // (Un)comment to rotate nut hole
	rotate([90, 0, 0])
		for(i = [0:(360 / 3):359])
		{
			rotate([0, 0, i])
				cube([4.6765, 8.1, 5], center = true);
		}
}

module flap(Width)
{
	rotate([90, 0, 0])
	union()
	{
		translate([3.5, (-7.5), 0])
			cube([4 + Extra_Mount_Depth, 15, Width]);


		translate([0, (-7.5), 0])
			cube([7.5 + Extra_Mount_Depth, 4, Width]);

		translate([0, 3.5, 0])
			cube([7.5 + Extra_Mount_Depth, 4, Width]);

		difference()
		{
			cylinder(h = Width, d = 15);

			translate([0, 0, (-1)])
				cylinder(h = Width + 2, d = 6);
		}
	}
}

module mount2()
{
	union()
	{

			translate([0, 4, 0])
		flap(3);

		translate([0, 10.5, 0])
		flap(3);
	}
}

module mount3()
{
	union()
	{
		difference()
		{
			translate([0, (-2.5), 0])
				flap(8);

			translate([0, (-8.5), 0])
				nut_hole();
		}

		mount2();
	}
}


