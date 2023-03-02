import sys
print("Content-Type: text/html", end="\r\n")
print("Set-Cookie: name=ayoub;", end="\r\n")
#print("Location: https://www.google.com", end="\r\n")
#print("Status: 301 Not Found", end="\r\n")
print(end="\r\n")

print("STDIN")
for line in sys.stdin:
    print(line)


print("Hello From CGI")
