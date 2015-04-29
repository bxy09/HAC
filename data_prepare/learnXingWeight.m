function learnXingWeight( dataPath,mlbPath )
%LEARNXINGWEIGHT Summary of this function goes here
%   Detailed explanation goes here
%%readData
%matlab store matrices by columns!!!
fid = fopen(dataPath,'r');
[line count] = fscanf(fid,'%d %d %d\n',3);
dataNum = line(1);
featureNum = line(2);
pairNum = line(3);
pairs = zeros(0,3);
for i=1:dataNum
    line = fgetl(fid);
    A = sscanf(line,'%d');
    assert(mod(length(A),2) == 0);
    
    rowi = A(1:2:length(A));
    s = A(2:2:length(A));
    s = s/sum(s);
    columnj = i*ones(size(rowi));
    pairs = [pairs;[rowi,columnj,s]];
end
data = sparse(pairs(:,1),pairs(:,2),pairs(:,3),featureNum,dataNum);

featureFre =  sum(data>0,2);
[i,j,s] = find(featureFre);
s = log(dataNum*s.^-1);
for k=1:dataNum
    data(:,k)=data(:,k).*s;
end
%load constraint
fclose(fid);
fid = fopen(mlbPath,'r');
A = fscanf(fid,'%d');
assert(mod(length(A),3)==0);
A = A + 1;
constraints = [A(1:3:length(A))';A(2:3:length(A))';A(3:3:length(A))'];
%get related docs
related = unique(A)';

%%%%%%%%
    function [tar] = target(w)
        B = sparse(featureNum,dataNum);
        size(data)
        size(w)
        for fi=related
            B(:,fi) = data(:,fi).*w;
            B(:,fi) = B(:,fi)/(sum(B(:,fi).^2).^0.5);
        end
        constraintNum = size(constraints,2)
        value = zeros(1,constraintNum);
        for fi = 1:constraintNum
            value(i) = sum(B(:,constraints(1,i)).*B(:,constraints(2,i)));
        end
        tar = -sum(value)
    end
    function [c,ceq] = condition(w)
        w
        B = sparse(featureNum,dataNum);
        for fi=related
            B(:,fi) = data(:,fi).*w;
            B(:,fi) = B(:,fi)/sum(B(:,fi).^2).^0.5;
        end
        constraintNum = size(constraints,2)
        value = zeros(1,constraintNum*2);
        for fi = 1:constraintNum
            value(2*i-1) = sum(B(:,constraints(1,i)).*B(:,constraints(3,i)));
            value(2*i) = sum(B(:,constraints(2,i)).*B(:,constraints(3,i)));
        end
        c = sum(value)-200
        ceq = 0;
    end
%options = optimoptions('fmincon','Hessian','user-supplied')
%options = optimoptions('fmincon','GradObj','on')
fmincon(@target,ones(featureNum,1),[],[],[],[],[],[],@condition)



end

