backcover();

cover_z = 2;

module backcover(){
    difference(){
        union(){
            backplate();
            nanoframe();
            translate([0, 0, 10.5 + cover_z/2])
            rotate([0, 90, -90])
                mount3();        
        }
        #translate([0,-15.5,0]) backhole();
    }
}

module backplate(){
    cube([35,34,cover_z], center = true);
}

module backhole(){
    cube([5,3,20], center = true);
}

module nanoframe(){
    //nano
    difference(){
    translate([0,0,-4.05/2])cube([35,34,cover_z], center = true);
    translate([0,0,-4.05/2])cube([34+2,18.5,4.05], center = true);
        translate([0,15,-4.05/2])cube([35,8,cover_z], center = true);
        mirror([0,1,0])translate([0,15,-4.05/2])cube([35,8,cover_z], center = true);
    }
}

module gopromount(){
    
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

/* GoPro Mount - 2 flap no nut hole
translate([0, 0, 10.5])
	rotate([0, 90, 0])
		mount2();
// */

//* GoPro Mount - 3 flap w/nut hole

// */