import os

for i in range(1, 11):
    cmd = './ep1 1 small_test' + str(i) + '.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_small' +str(i)+ '.txt')
    
    cmd = './ep1 1 medium_test' + str(i) + '.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_medium' +str(i)+ '.txt')
    
    cmd = './ep1 1 large_test' + str(i) + '.txt output.txt'
    os.system(cmd)
    os.rename('output.txt','output_large' +str(i)+ '.txt')