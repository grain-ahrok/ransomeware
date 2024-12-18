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

## 프로젝트 개요
### 요약
최근 RaaS 서비스의 확산으로 개인과 기업에 랜섬웨어를 이용한 보안 위협이 증가하고 있습니다. 이런 상황을 대해 잘 이해하고 대응하기 위해, 랜섬웨어에 사용되는 기술들을 연구하고 개발하였습니다. 이를 통해, 실제 랜섬웨어 작동 원리를 체계적으로 분석하고, 보안 취약점을 미리 파악하여 보다 효과적인 전략을 마련하고자 합니다.
### 스킬
- C/C++
- Win32 API, Win32 GUI, WinSock
- Linux
- AWS EC2

</br>
</br>

## 동작 시나리오
### C&C 서버
- IP, MAC 주소 기반으로 기존 감염 여부 확인
- 접근 로그 저장 및 안전한 키보관
- Thread 기능을 활용하여 다중 연결 가능

### 하이브리드 암호화 기법
<img width="1086" alt="스크린샷 2024-04-03 오후 8 30 35" src="https://github.com/grain-ahrok/ransomeware/assets/81209784/327707f5-aed9-46a8-8cb2-fb56b6501de3">

- 서버
  - 새로운 클라이언트 접속 시, RSA 키 쌍 생성 후 공개키 전달
  - 코인 지불 확인 후, 개인키 전달

- 클라이언트
  - 디렉토리 별로 AES 키 생성 후, 파일 암호화
  - 생성한 AES 키를 RSA 공개키로 암호화 하여 dirkey.text 에 저장
  - 코인 지불 후, 개인키를 전달 받아 복호화

</br>
</br>

## 특징
### 안티리버싱
- CPU 벤더란 CPU 제조사를 확인할 수 있는 식별 정보이다. 가상환경에서는 가상머신의 하이퍼바어지가 CPU를 에뮬레이트 하므로 하드웨어 정보를 수정하여 CPU 벤더를 수정할 수 있다.
- CPU Vendor 확인을 통해, 가상환경에서 랜섬웨어 동작시 자동 종료

| CPU 벤더 문자열 |	가상 환경 |
| --- | --- |
| `KVMKVMKVM` | KVM (Kernel-based Virtual Machine) |
| `Microsoft Hv` | Microsoft Hyper-V | 
| `VMwareVMware` | VMware Workstation/ESXi | 
| `XenVMMXenVMM` | Xen Hypervisor |
| `VBoxVBoxVBox` | VirtualBox |
| `Parallels` | Parallels Desktop |
| `QEMU` | QEMU (Quick Emulator) | 


### 레지스트리 키 설정
- `"Software\\Microsoft\\Windows\\CurrentVersion\\Run"`에 "chrmoe"으로 실행 파일 등록
- 컴퓨터 재부팅 시 랜섬웨어 자동 실행

### 자동 복원 되는 파일 시그니처
| jpeg | FF D8 FF E0 FF D8 FF E8 |
| --- | --- |
| gif | 47 49 46 38 37 61 47 49 46 38 39 61 |
| png | 89 50 4E 47 0D 0A 1A 0A |
| pdf | 25 50 44 46 2D 31 2E |
| zip | 50 4B 03 04 |
| RAR | 52 61 72 21 1A 07 |
| DOCX, PPTX, XLSX | 50 4B 03 04 14 00 06 00 | 


### 랜섬웨어 배포 방법 제시
- CVE-2023-38831 : WinRAR 6.22 취약점
- Window 파일명 규칙과 WinRAR 파일명 규칙의 차이점 이용
- WinRAR은 동일한 파일명을 가진 여러 파일을 하나로 압축할 수 있다. 이를 악용하면 랜섬웨어가 직접 실행되지 않더라도, 동일한 이름의 다른 파일을 실행하도록 유도하여 악성코드를 실행할 수 있다.

https://github.com/user-attachments/assets/f9096455-f469-4832-9dfd-23f01d75588b


<br/>
<br/>


## 악성프로그램 실행
https://github.com/grain-ahrok/ransomeware/assets/81209784/248a94de-f872-42ac-8a5c-7e1964ebc6ce

<br/>
<br/>


##  추후 개발 목록
- [ ] 백업 파일 삭제
- [ ] 토르 브라우저 사용
- [ ] 랜섬웨어 코드 난독화
