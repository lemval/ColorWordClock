// Copyright (c) 2026 - Lemval

clockSize = 300;     // How large is the clock (square)
step = 16.6667;      // The length between each LED center
dotsPerLine = 13;    // Number of LEDs per line 
dotMargin = 27;      // Distance dots from side
lineSize = 12;       // Width of the LED strip
dotSize = 5;         // Size of the LED dot
leader = 12.5;       // Space left and right for soldering
screwSize = 3.25;    // M3 with some slack
resCircle = 15;      // Smoothen circles 
resText = 50;        // Smoothen text
screwOffset = 14.5;  // Location of the corner screws
deviceWidth = 54;    // Width of the ESP
deviceHeight = 32;   // Height of the ESP
deviceThickness = 7; // Thickness of the ESP

// Glueing magnets to the front plate requires non-invasive glue!
// magnetSize = 5.1;
// magnetOffset = 20;

lettersDutch="HET!ISXEENOBMTWEEDRIEKVIERVIJFZESZEVENPACHTDXWNEGENYTIENELFTWAALFKWARTLDERTIENVEERTIENMVOOROVERMHALFDEENTWEEBDRIEVIERVIJFZESZEVENLACHTNEGENTIENELFTWAALFYUUR▪▪▪▪▪▪▪▪▪▪▪▪▪";
lettersEnglish= "IT!ISXONEATWOTHREEFOURFIVESIXSEVENEIGHTNINETENELEVENTWELVEQUARTERTHIRFOURTEENBHALFLPASTATOMONELTWOBTHREEFOURZFIVEDSIXSEVENEIGHTTENNINEELEVENLEMTWELVEXTHIRTY▪▪▪O▪CLOCK▪▪▪";

// TODO Choose the correct language
// If you use another layout (other language), make sure you work the code.
letters=lettersEnglish;

fontSize=12;
fontName="Cliche:style=regular";

// There are different layers to cut. Generate or use existing ones.
// The power line uses a USB C socket. Change the model for other sockets.
// Speed/power/passes are based on a 20W diode laser.

// printGroundLayer();   // Basewood 6 mm - 300mm/m, 100%, 2 passes
// printEngraveLayer();  // On ground layer - 3000mm/m, 20%, 1 pass
// printLedStripLayer(); // MDF 2 mm - 400mm/m, 100%, 1 pass
// printControlLayer();  // MDF 6 mm - 300mm/m, 100%, 3 passes
// printTextLayer();     // Lasersheet 0.5mm - 600mm/m, 90%, 1 pass

module printGroundLayer() {
    difference() {
        square(clockSize);
        Screws();
    }
}

module printEngraveLayer() {
    // This layer can be used for aligning LEDs on LED layer
    width=0.1;
    // Center line
    translate([clockSize/2, clockSize/2, 0]) square([width, clockSize], center=true);
    // Dot lines, left vertical
    translate([dotMargin, clockSize/2, 0]) square([width, clockSize], center=true);
    // Dot lines, right vertical
    translate([clockSize-dotMargin, clockSize/2, 0]) square([width, clockSize], center=true);
    // Dot lines, top horizontal
    translate([clockSize/2, clockSize-dotMargin, 0]) square([clockSize, width], center=true);
    // Dot lines, bottom horizontal
    translate([clockSize/2, dotMargin, 0]) square([clockSize, width], center=true);
    translate([dotMargin+5, 12, 0]) text("(c) 2026 Lemval", font=fontName, size=6, $fn=50);      
}

module printLedStripLayer() {
    // Carved for strips and wiring
    difference() {
        square(clockSize);
        union() {
            for(idx = [0 : 1 : dotsPerLine-1]) 
                StripLed(idx, false);
            Screws();
            Device();
            DotWires();
            PowerConnector();
        }
    }
}

module printControlLayer() {
    // Carved for device and light boxes
    // mirror([180,0,0]) 
    difference() {
        square(clockSize);
        union() {
            // Add the text squares
            for(idx = [0 : 1 : dotsPerLine-1]) 
                Text(idx, true);
            Screws();
            Device();
            LightSensor();
            Switches();
            PowerInlet();
            Dots(true);   
            // Magnets(); 
        }
    }
}

module printTextLayer() {
    // Carved for text
    difference() {
        square(clockSize);
        union() {
             Dots(false);     
             for(idx = [0 : 1 : dotsPerLine-1]) Text(idx, false);
            // Magnets(); 
            // Bolts();
        }
    }
}

module Device() {
    translate([44, clockSize-39, 0]) 
        square([deviceWidth+20,deviceHeight]);
}

module LightSensor() {
    // Space on top. Drill when needed for wires
    translate([clockSize/2, clockSize-1, 0]) 
        square([4.2,2], true);
    // Vertical portal
    translate([clockSize/2, clockSize-18, 0]) 
        square([4,20], true);
    // Horizontal portal
    translate([45+deviceWidth, clockSize-31, 0]) 
        square([clockSize/2-45-deviceWidth+2,4]);
}

module PowerConnector() {
    // Conduit
    translate([clockSize/2, 15]) square([10, 20], true);
    // Extra wire space
    translate([clockSize/2, 18]) square([18, 9], true);
}

module PowerInlet() {
    translate([clockSize/2-10, 0]) square([20, 2.5]);
    translate([clockSize/2, 10]) square([10, 20], true);
}

module Switches() {
    // Small holes should be drilled from the side.
    offset = 17;
    // Switch 1
    translate([offset, clockSize-15,0]) square([7,10], false);
    // Switch 2
    translate([offset+13, clockSize-15,0]) square([7,10], false);
    // Switch wire
    translate([offset, clockSize-17,0]) square([28,3], false);
}

module StripLed(row, showDots) {
    length = leader + dotsPerLine * step;
    height = dotsPerLine * step - (lineSize/2);
    startX = (clockSize - length) / 2;
    startY = (clockSize - height) / 2;
    yPos = startY + row * step;
    
    translate([startX, yPos, 0]) square([length, lineSize]);
    
    if (showDots) {    
        yStart = yPos + lineSize/2;
        xStart = (clockSize - dotsPerLine * step + leader + dotSize)/2;
        for(idx = [0 : 1 : dotsPerLine - 1]) {
           translate([xStart + idx * step, yStart, 0]) square(dotSize, true);
        }
    }
    
    drawWireSpace(startX, yPos, row%2 == 0);
}

module drawWireSpace(x, y, isEven) {
    startX = isEven ? x - 7 : x + dotsPerLine * step + 3;
    translate([startX,y,0]) square([step, step+lineSize]);
}

module Text(row, showBlocks) {
    base = (clockSize - dotsPerLine * step) / 2;
    startX = base + (leader + dotSize)/2;
    startY = base + (lineSize + dotSize)/2 + 0.5 + (dotsPerLine-1-row) * step;
    
    for(idx = [0 : 1 : dotsPerLine-1]) {
        character = letters[idx + dotsPerLine * row];
        translate([startX + idx * step, startY, 0]) {
            if (showBlocks) {
                sSize = fontSize+1;
                square(sSize, true);
            } else {
                text(character, 
                     size=fontSize,
                     font=fontName,
                     halign="center",
                     valign="center",
                     $fn=50);
            }      
        }
    }
}

module DotWires() {
    max = clockSize - dotMargin;
    min = dotMargin;
    width = lineSize + 18;
    height = lineSize + 2;
    spacer = 20;
    length = max - min - 2*spacer;

    // Boxes for the dot elements
    translate([min+2, max-1, 0]) square([width, height],true);
    translate([max-2, max-1, 0]) square([width, height],true);
    translate([max-2, min+1, 0]) square([width, height],true);
    translate([min+2, min+1, 0]) square([width, height],true);
    
    // Bottom wire section
    translate([min+10, min-6, 0]) square([length+20, 7]);
    // Top wire section
    translate([min+10, max-1, 0]) square([length+20, 7]);
    // Left wire section
    translate([min-13, min+spacer-8, 0]) square([7, length+15]);
    
    drawWireSpace(max-11, min+1, true);
}

module Dots(blocks) {
    max = clockSize - dotMargin;
    min = dotMargin;
    translate([min, max, 0]) 
        if (blocks) square(fontSize+1, true); else circle(3,true, $fn=20);
    translate([max, max, 0])
        if (blocks) square(fontSize+1, true); else circle(3,true, $fn=20);
    translate([max, min, 0])
        if (blocks) square(fontSize+1, true); else circle(3,true, $fn=20);
    translate([min, min, 0])
        if (blocks) square(fontSize+1, true); else circle(3,true, $fn=20);
}

module Screws() {
    max = clockSize - screwOffset;
    min = screwOffset;
    delta = 20;
    xLeft = clockSize/2-delta;
    xRight = clockSize/2+delta;
    translate([min, max, 0]) circle(d=screwSize, $fn=resCircle);
    translate([max, max, 0]) circle(d=screwSize, $fn=resCircle);
    translate([max, min, 0]) circle(d=screwSize, $fn=resCircle);
    translate([min, min, 0]) circle(d=screwSize, $fn=resCircle);
    
    translate([clockSize/2, 30, 0]) circle(d=screwSize, $fn=resCircle);
    translate([xLeft, max, 0]) circle(d=screwSize, $fn=resCircle);
    translate([xRight, max, 0]) circle(d=screwSize, $fn=resCircle);
}

module Magnets() {
    space = clockSize/4;
    left = magnetOffset;
    right = clockSize - magnetOffset;
    bottom = left;
    top = right;
    p1 = space;
    p3 = 3*space;
    
    translate([left, p3, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([left, p1, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([right, p3, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([right, p1, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([p1, top, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([p3, top, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([p1, bottom, 0]) circle(d=magnetSize, $fn=resCircle);
    translate([p3, bottom, 0]) circle(d=magnetSize, $fn=resCircle);
}

module Bolts() {
    max = clockSize - screwOffset;
    min = screwOffset;
    size = 5.5;
    small = size/2.0 * 1.0/cos(30);

    translate([min, max, 0]) union() {
        square([size, small], center=true);
        rotate([0,0,60]) square([size, small], center=true);
        rotate([0,0,120]) square([size, small], center=true);
    }
    translate([max, max, 0]) union() {
        square([size, small], center=true);
        rotate([0,0,60]) square([size, small], center=true);
        rotate([0,0,120]) square([size, small], center=true);
    }
    translate([min, min, 0]) union() {
        square([size, small], center=true);
        rotate([0,0,60]) square([size, small], center=true);
        rotate([0,0,120]) square([size, small], center=true);
    }
    translate([max, min, 0]) union() {
        square([size, small], center=true);
        rotate([0,0,60]) square([size, small], center=true);
        rotate([0,0,120]) square([size, small], center=true);
    }
}


