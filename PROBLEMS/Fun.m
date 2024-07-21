function z=Fun(u)
% if Benchmark_Function_ID==1 
% Sphere function with fmin=0 at (0,0,...,0)
z=sum(u.^2);
% if Benchmark_Function_ID==2 
% z=sum(abs(u))+prod(abs(u));
% end
% if Benchmark_Function_ID==3
% d=1000;
%     z=0;
%     for i=1:100
%     z=z+sum(u(1:i))^2;
%     end
% end
% 
% if Benchmark_Function_ID==4
%     z=max(abs(u));
% end
% F5
% z=sum(100*(u(2:30)-(u(1:30-1).^2)).^2+(u(1:30-1)-1).^2);
% if Benchmark_Function_ID==6
%     z=sum(abs((u+.5)).^2);
% end
% 
% if Benchmark_Function_ID==7
%     z=sum([1:30].*(u.^4))+rand;
% end
% 
% if Benchmark_Function_ID==8
%     z=sum(-u.*sin(sqrt(abs(u))));
% end
% 
% if Benchmark_Function_ID==9
%     z=sum(u.^2-10*cos(2*pi.*u))+10*30;
% end
% F10
% z=-20*exp(-.2*sqrt(sum(u.^2)/30))-exp(sum(cos(2*pi.*u))/30)+20+exp(1);
% if Benchmark_Function_ID==11
%     z=sum(u.^2)/4000-prod(cos(u./sqrt([1:30])))+1;
% end
% F12
%  z=(pi/30)*(10*((sin(pi*(1+(u(1)+1)/4)))^2)+sum((((u(1:30-1)+1)./4).^2).*...
%          (1+10.*((sin(pi.*(1+(u(2:30)+1)./4)))).^2))+((u(30)+1)/4)^2)+sum(Ufun(u,10,100,4));
% if Benchmark_Function_ID==13
%     z=.1*((sin(3*pi*u(1)))^2+sum((u(1:30-1)-1).^2.*(1+(sin(3.*pi.*u(2:30))).^2))+...
%          ((u(30)-1)^2)*(1+(sin(2*pi*u(30)))^2))+sum(Ufun(u,5,100,4));
% end
% 
% if Benchmark_Function_ID==14
% aS=[-32 -16 0 16 32 -32 -16 0 16 32 -32 -16 0 16 32 -32 -16 0 16 32 -32 -16 0 16 32;...
% -32 -32 -32 -32 -32 -16 -16 -16 -16 -16 0 0 0 0 0 16 16 16 16 16 32 32 32 32 32];
%     for j=1:25
%         bS(j)=sum((u'-aS(:,j)).^6);
%     end
%     z=(1/500+sum(1./([1:25]+bS))).^(-1);
% end
% 
% if Benchmark_Function_ID==15
%     aK=[.1957 .1947 .1735 .16 .0844 .0627 .0456 .0342 .0323 .0235 .0246];
%     bK=[.25 .5 1 2 4 6 8 10 12 14 16];bK=1./bK;
%     z=sum((aK-((u(1).*(bK.^2+u(2).*bK))./(bK.^2+u(3).*bK+u(4)))).^2);
% end
% 
% if Benchmark_Function_ID==16
%     z=4*(u(1)^2)-2.1*(u(1)^4)+(u(1)^6)/3+u(1)*u(2)-4*(u(2)^2)+4*(u(2)^4);
% end
% 
% if Benchmark_Function_ID==17
%     z=(u(2)-(u(1)^2)*5.1/(4*(pi^2))+5/pi*u(1)-6)^2+10*(1-1/(8*pi))*cos(u(1))+10;
% end
% F18
% z=(1+(u(1)+u(2)+1)^2*(19-14*u(1)+3*(u(1)^2)-14*u(2)+6*u(1)*u(2)+3*u(2)^2))*...
%         (30+(2*u(1)-3*u(2))^2*(18-32*u(1)+12*(u(1)^2)+48*u(2)-36*u(1)*u(2)+27*(u(2)^2)));
% if Benchmark_Function_ID==19
%     aH=[3 10 30;.1 10 35;3 10 30;.1 10 35];cH=[1 1.2 3 3.2];
%     pH=[.3689 .117 .2673;.4699 .4387 .747;.1091 .8732 .5547;.03815 .5743 .8828];
%     z=0;
%     for i=1:4
%     z=z-cH(i)*exp(-(sum(aH(i,:).*((u-pH(i,:)).^2))));
%     end
% end

% if Benchmark_Function_ID==20
%     aH=[10 3 17 3.5 1.7 8;.05 10 17 .1 8 14;3 3.5 1.7 10 17 8;17 8 .05 10 .1 14];
% cH=[1 1.2 3 3.2];
% pH=[.1312 .1696 .5569 .0124 .8283 .5886;.2329 .4135 .8307 .3736 .1004 .9991;...
% .2348 .1415 .3522 .2883 .3047 .6650;.4047 .8828 .8732 .5743 .1091 .0381];
%     z=0;
%     for i=1:4
%     z=z-cH(i)*exp(-(sum(aH(i,:).*((u-pH(i,:)).^2))));
%     end

% aSH=[4 4 4 4;1 1 1 1;8 8 8 8;6 6 6 6;3 7 3 7;2 9 2 9;5 5 3 3;8 1 8 1;6 2 6 2;7 3.6 7 3.6];
% cSH=[.1 .2 .2 .4 .4 .6 .3 .7 .5 .5];

% if Benchmark_Function_ID==21
%     z=0;
%   for i=1:5
%     z=z-((u-aSH(i,:))*(u-aSH(i,:))'+cSH(i))^(-1);
%   end
% end

% if Benchmark_Function_ID==22
%     z=0;
%   for i=1:7
%     z=z-((u-aSH(i,:))*(u-aSH(i,:))'+cSH(i))^(-1);
%   end
% end
% 
% if Benchmark_Function_ID==23
%     z=0;
%   for i=1:10
%     z=z-((u-aSH(i,:))*(u-aSH(i,:))'+cSH(i))^(-1);
%   end
% end

%%%%% ============ end ====================================