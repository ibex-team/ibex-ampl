var x1 >= -100, <= 100;
var x2 >= -100, <= 100;
var x3 >= -100, <= 100;
var x4 >= -100, <= 100;
var x5 >= -100, <= 100;
var x6 >= -100, <= 100;
var x7 >= -100, <= 100;
var x8 >= -100, <= 100;
var x9 >= -100, <= 100;
var x10 >= -100, <= 100;
var x11;
var x12;
var x13;



subject to con1 :  x1  - 0.18324*(x4*x3*x9)**3  + x3**4*x9**7  - 0.25428=0;
subject to con2 :  x2  - 0.16275*(x1*x10*x6)**3 + x10**4*x6**7 - 0.37842=0;
subject to con3 :  x3  - 0.16955*(x1*x2*x10)**3 + x2**4*x10**7 - 0.27162=0;
subject to con4 :  x4  - 0.15585*(x7*x1*x6)**3  + x1**4*x6**7  - 0.19807=0;
subject to con5 :  x5  - 0.19950*(x7*x6*x3)**3  + x6**4*x3**7  - 0.44166=0;
subject to con6 :  x6  - 0.18922*(x8*x5*x10)**3 + x5**4*x10**7 - 0.14654=0;
subject to con7 :  x7  - 0.21180*(x2*x5*x8)**3  + x5**4*x8**7  - 0.42937=0;
subject to con8 :  x8  - 0.17081*(x1*x7*x6)**3  + x7**4*x6**7  - 0.07056=0;
subject to con9 :  x9  - 0.19612*(x10*x6*x8)**3 + x6**4*x8**7  - 0.34504=0;
subject to con10 : x10 - 0.21466*(x4*x8*x1)**3  + x8**4*x1**7  - 0.42651=0;
subject to con11 : x11 + x12 + x13 =0;

option auxfiles rc;
write gex8; 

