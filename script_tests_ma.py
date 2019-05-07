import random

def generate_random_string(length):
        random_string = ''
        random_str_seq = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        for i in range(0,length):
            if i % length == 0 and i != 0:
                random_string += '-'
            random_string += str(random_str_seq[random.randint(0, len(random_str_seq) - 1)])
        return random_string


prog = ["n","p"]

for x in range(200):
  x = random.randint(0,1)
  y = (prog[x])
  if(y == "p"):
    cod =  (random.randint(0,66))
    novoPreco = random.randint(0,15000)
    novoPrecoFloat = random.randint(0,99)
    print("%s %s %s" % (y,cod,novoPreco))

  if(y == "n"):
      cod =  (random.randint(0,66))
      novoNome = generate_random_string(random.randint(4,12))
      print("%s %s %s" % (y,cod, novoNome))
  
  

