class MyStr {
  public MyStr() {
    content = new char[0]; 
  }

  public MyStr(char[] str) {
    content = new char[str.length];
    for (int i = 0;i < str.length; ++i)
      content[i] = str[i];
  }

  public MyStr(String str) {
    content = new char[str.length()];
    for (int i = 0;i < str.length(); ++i)
      content[i] = str.charAt(i);
  }

  public boolean empty() {
    return content.length == 0 ? true : false;
  }

  public void print() {
    for (char ch: content)
      System.out.print(ch); 
  }

  public int length() {
    return content.length; 
  }

  public int find(char ch) {
    for (int i = 0;i < content.length; ++i)
      if (content[i] == ch)
        return i;
    return -1; 
  }

  public char at(int i) {
    if (i <= 0) 
      return content[0];
    else if (i >= content.length) 
      return content[content.length-1];
    else 
    return content[i];
  }

  public int compare(MyStr str) {
    int len = content.length > str.length() ? 
              str.length(): content.length;

    for (int i = 0;i < len;i ++) {
      if (content[i] < str.at(i)) 
        return -1;
      else if (content[i] > str.at(i))
        return 1;
    }

    if (content.length > len) return 1;
    if (str.length() > len) return -1;

    return 0;
  }

  public void append(char ch) {
    char[] tmp = content;
    content = new char[content.length+1];
    for (int i = 0;i < tmp.length; ++i)
      content[i] = tmp[i];
    content[content.length-1] = ch;
  }

  public void append(MyStr str) {
    char[] tmp = content;
    content = new char[content.length + str.length()];
    for (int i = 0;i < tmp.length; ++i)
      content[i] = tmp[i];
    for (int i = 0;i < str.length(); ++i)
      content[tmp.length + i] = str.at(i);
  }

  public void copy(MyStr str) {
    content = new char[str.length()];
    for (int i = 0;i < content.length; ++i)
      content[i] = str.at(i);
  }

  public MyStr slice(int start, int end) {
    if (start > content.length || end < 0 || start > end)
      return null;
    int start_pos = start < 0 ? 0 : start;
    int end_pos = end > content.length ? content.length : end;

    MyStr ret = new MyStr();
    for (int i = start_pos;i < end_pos; ++i)
      ret.append(content[i]);

    return ret;
  }

  public MyStr odd() {
    MyStr ret = new MyStr(); 

    for (int i = 1;i <= content.length; ++i)
      if (i % 2 == 1)
        ret.append(content[i-1]);

    return ret;
  }

  private char[] content;
}

public class hw1 {
  public static void main(String[] args) {
    MyStr emptyObj = new MyStr();
    MyStr oneCharObj = new MyStr("a");
    MyStr oddCharsObj = new MyStr(new char[]{'a', 'b', 'c', 'd', '1', '2', '3'});
    MyStr evenCharsObj = new MyStr("abcd1234%^&*");

    System.out.print("- Test 1, input sequence is empty");
    System.out.print(", output sequence is: ");
    emptyObj.odd().print();
    if (emptyObj.odd().compare(emptyObj) == 0)
      System.out.println(". Test 1 passed.");

    System.out.print("- Test 2, input sequence is ");
    oneCharObj.print();
    System.out.print(", output sequence is: ");
    oneCharObj.odd().print();
    if (oneCharObj.odd().compare(oneCharObj) == 0)
      System.out.println(". Test 2 passed.");
    
    System.out.print("- Test 3, input sequence is ");
    oddCharsObj.print();
    System.out.print(", output sequence is: ");
    oddCharsObj.odd().print();
    if (oddCharsObj.odd().compare(new MyStr("ac13")) == 0)
      System.out.println(". Test 3 passed.");

    System.out.print("- Test 4, input sequence is ");
    evenCharsObj.print();
    System.out.print(", output sequence is: ");
    evenCharsObj.odd().print();
    if (evenCharsObj.odd().compare(new MyStr("ac13%&")) == 0)
      System.out.println(". Test 4 passed.");
  }
}


