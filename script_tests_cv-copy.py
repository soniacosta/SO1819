import random

def generate_random_string(length):
        random_string = ''
        random_str_seq = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        for i in range(0,length):
            if i % length == 0 and i != 0:
                random_string += '-'
            random_string += str(random_str_seq[random.randint(0, len(random_str_seq) - 1)])
        return random_string


for x in range(200):
  x = random.randint(1,3)
  
  if(x == 1):
    cod =  (random.randint(0,60))
    
    print("%s" % (cod))

  if(x == 2):
      cod =  (random.randint(0,60))
      sinal = ["","-"]
      si = random.randint(0,1)
      stock = random.randint(0,500)
      
      print("%s %s%s" % (cod, sinal[si], stock))
  

