import os

for i in range(1, 31):
    cmd = './ep1 1 small_test1.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_small' +str(i)+ '.txt')
    
    cmd = './ep1teste 1 medium_test1.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_medium' +str(i)+ '.txt')
    
    cmd = './ep1teste 1 large_test1.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_large' +str(i)+ '.txt')