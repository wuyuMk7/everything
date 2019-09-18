all: hw1-c hw1-java

hw1-c: hw1.c
	$(CC) -Wall $< -o $@

hw1-java: hw1.java
	javac hw1.java 
clean:
	rm -rf hw1-c hw1.class MyStr.class
