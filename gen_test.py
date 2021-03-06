from random import seed, randint

seed(1234567890)

def gen_test(name, max_start_time, max_duration, max_deadline, size):
    process_list = []
    for i in range(0, size):
        t0 = randint(0, max_start_time)
        dt = randint(1, max_duration)
        deadline = t0 + dt + randint(1, max_deadline)
        process_list.append((t0, dt, deadline))

    process_list.sort(key=lambda p:p[0])
 
    with open(name, 'w') as f:
        i = 1
        for t0, dt, deadline in process_list:
            f.write("proc{} {} {} {}\n".format(i, t0, dt, deadline))
            i += 1

for i in range(1, 11):
    gen_test('small_test{}.txt'.format(i), 15, 30, 100, 10)      # max_duration = 30 sec
    gen_test('medium_test{}.txt'.format(i), 30, 30, 200, 100)   # max_duration = 60 sec
    gen_test('large_test{}.txt'.format(i), 60, 30, 300, 1000) # max_duration = 60 sec
