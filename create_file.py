def create_file(name ,size):
    f = open(name , "a")
    for i in range(0,size):
           f.write("a")

create_file("1kb.txt" , 1000)
create_file("100kb.txt" , 100000)
create_file("1mb.txt" , 1000000)
create_file("10mb.txt" , 10000000)
create_file("100mb.txt" , 100000000)

