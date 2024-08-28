## ransomeware

<b> Warning: Unauthorized Use of This Ransomware Project </b>

This project is intended solely for educational purposes to understand the mechanisms of ransomware. Unauthorized use, distribution, or deployment of this software for malicious activities is strictly prohibited and against the law.

<b> Important Notice: </b>

<b> Legal Liability: </b> If you choose to use, modify, or distribute this software, you do so at your own risk. You are solely responsible for any legal consequences, damages, or actions arising from its misuse. The author of this project will not be held liable for any illegal activities conducted with this software.

<b> Ethical Considerations: </b> Misuse of ransomware can cause significant harm and distress. Please consider the ethical implications and potential impact on individuals and organizations.

<b> Strict Prohibition: </b> Any unauthorized use of this project for malicious purposes is strictly forbidden. This software is provided "as-is" for educational and research purposes only.

By using this software, you acknowledge that you understand the risks and responsibilities involved.


</br>
</br>

### 동작 시나리오
**서버** <br/>
새로운 클라이언트 접속 시, RSA 키 쌍 생성 후 공개키 전달.<br/>
코인 지불 확인 후, 개인키 전달.<br/>

**클라이언트**<br/>
디렉토리 별로 AES 키 생성 후, 파일 암호화<br/>
생성한 AES 키를 RSA 공개키로 암호화 하여 dirkey.text 에 저장<br/>
코인 지불 후, 개인키를 전달 받아 복호화<br/>
<img width="1086" alt="스크린샷 2024-04-03 오후 8 30 35" src="https://github.com/grain-ahrok/ransomeware/assets/81209784/327707f5-aed9-46a8-8cb2-fb56b6501de3">



### 특징
- **하이브리드 암호화 기법**
  1. 파일 암호화에 AES를 사용하여 암호 연산 시간을 단축하였습니다.
  2. 암호화에 사용된 AES 키는 서버에서 전달받은 RSA 공개키를 사용하여 클라이언트 컴퓨터에 암호화하여 저장하였습니다.
  3. 서버에서 RSA 개인키를 전달 받은 후에는 안전하게 복구가 가능합니다.
- **특정 조건에서 실행되도록 설정** <br/>
  CPU Vendor 확인을 통해, 가상환경에서 실행하는 경우 동작하지 않습니다.<br/>
  https://ah-rok.tistory.com/58 
- **재부팅시 자동 실행**<br/>
  `"Software\\Microsoft\\Windows\\CurrentVersion\\Run"` 에 실행 파일 등록하여 재부팅 시 자동실행됩니다.

<br/><br/>
### 자동 복원 되는 파일 시그니처
| jpeg | FF D8 FF E0 FF D8 FF E8 |
| --- | --- |
| gif | 47 49 46 38 37 61 47 49 46 38 39 61 |
| png | 89 50 4E 47 0D 0A 1A 0A |
| pdf | 25 50 44 46 2D 31 2E |
| zip | 50 4B 03 04 |
| RAR | 52 61 72 21 1A 07 |
| DOCX, PPTX, XLSX | 50 4B 03 04 14 00 06 00 | 

<br/><br/>



### WinRAR 취약점 이용
취약점 내용 : https://ah-rok.tistory.com/56

https://github.com/grain-ahrok/ransomeware/assets/81209784/4206025b-ab6a-4c3c-9bdc-c2ce2b6b7ad9

<br/>
<br/>




### 악성프로그램 실행
https://github.com/grain-ahrok/ransomeware/assets/81209784/248a94de-f872-42ac-8a5c-7e1964ebc6ce

<br/>
<br/>


### 추후 개발 목록
- [ ] 백업 파일 삭제
- [ ] 토르 브라우저 사용
- [ ] 난독화



