with open('mama_table', 'ab') as f:
    s = b'hello bitches'.rjust(25, b'\0')
    f.write(s)
    
    age = 19
    f.write(age.to_bytes(4, signed=True))
