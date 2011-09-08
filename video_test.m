
clear all; clc; close all;

webcam_threaded();
locked = 0;
stopped = 0;
i  = 1;

pause(2);
figure

im = uint8(zeros(240,320,3,1));
while ( stopped < 0.5 )
    disp(i)
    if (locked > 0.5);
        if ( i < 1000 )
            %im(:,:,:,i) = uint8(image);
            i = i + 1;
        end
        imshow(uint8(image));
        pause(0.15);
        locked = 0;
    end
end
