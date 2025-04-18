function in = IsInCurvedRich(z,eta)
    
    Theta0 = 2*atan(exp(-eta));
    z_inner = 191.27840756391092 - 83.84605684233031*(-0.4 + Theta0) - 170.66757313824704*power(-0.4 + Theta0,2);
    z_outer = 283.38450696419943 - 122.78789107319537*(-0.4 + Theta0) - 216.7206228383916*power(-0.4 + Theta0,2);
    
    in =  (z>z_inner & z<z_outer);
        
end