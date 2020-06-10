#include <stdio.h>

int main() {
	_asm {
		mov eax, fs:0x30
		mov eax, [eax + 12]
		mov eax, [eax + 20]
		mov eax, [eax]
		mov eax, [eax]
		mov ebx, [eax + 16]
		mov dword ptr[ebp + 10], ebx					//base주소
		add ebx, 0x3c
		mov eax, [ebx]
		mov[ebp + 20], eax						//pe header offset
		mov ebx, [ebp + 10]
		add ebx, eax
		add ebx, 0x78
		mov ebx, [ebx]
		mov[ebp + 30], ebx						//export_table_rva
		mov eax, [ebp + 30]						//export_table_rva를 eax담기
		mov ecx, [ebp + 10]						//base주소를 ecx에 담기
		lea eax, [eax + ecx + 0x20]
		mov dword ptr[ebp-10], eax								// 주소 배열  

		mov eax, dword ptr[ebp + 30]
		mov ecx, dword ptr[ebp + 10]
		lea edx, [ecx + eax + 0x14]													// number_of_name   밑에 보면 알겠지만 그 01  11  이런식으로 있어서 lea쓰는겨
		// edx에 모셔놓는다.

		mov eax, [ebp-10]																//name_pointer_table_int
		mov ecx, [ebp + 10]
		add ecx, dword ptr[eax]
		mov dword ptr[ebp-30], ecx						// pointer table 아 주석에 사진을 뛰울수있으면 좋을텐뎅 미래의 희찬아 그냥 기억해 ㅎㅎ
																									// pe viewer으로 보면 있음

		mov eax, [ebp-30]
		mov edx, [eax]
		xor esi, esi
		find_winexec :																	//겁나 힘들게 구현한 name_pointer_table에서 WinExec가 몇번인지 찾기 후후
			inc esi
			mov ecx, [ebp-30]
			mov edx, dword ptr[ecx + esi * 4]							// 정수형  배열이라고 생각을 하셈. a = [1,2,3]   이라고 있다면 하나씩 증가해서 비교한다는 거임
			add edx, [ebp+10]
			//edx에 함수명들을 모셔놓기
			mov [ebp+150],0x57				//W
			mov [ebp+151], 0x69			//i
			mov [ebp+152], 0x6e			//n
			mov [ebp+153], 0x45			//E
			mov [ebp+154], 0x78			//x
			mov [ebp+155], 0x65			//e
			mov [ebp+156], 0x63			//c
			mov [ebp+157], 0					// \0
			lea eax, [ebp+150]

			//mov eax,dword ptr [eax]
			//	mov edx, dword ptr[edx]
			//	mov esi, dword ptr[esi]
			//	mov ecx, dword ptr[ecx]
			mov [ebp-110], eax
			mov [ebp-120], edx
			mov [ebp - 130], esi
			mov [ebp - 140], ecx

			mov esi, eax
			mov edi, edx
		first :
			mov dh, byte ptr[esi]
			mov dl, byte ptr[edi]
			cmp dh, dl
			je same_test
			ja over
			jb _low
		same_test :
			inc esi
			inc edi
			cmp dh, 0
			je same
			jmp first
		over :
			mov eax, 1;
			jmp endd
		_low :
			mov eax, -1;
			jmp endd
		same :
			mov eax, 0;
			jmp endd

		endd :

		mov edx, [ebp - 120]
		mov esi, [ebp - 130]
		mov ecx, [ebp - 140]

		cmp eax, 0
		jne find_winexec
		mov dword ptr[ebp-50], esi


		mov eax, dword ptr[ebp + 30]						// export_table_rva
		mov ecx, [ebp+10]
		lea edx, [ecx + eax + 0x24]									// 여러개로 나열이 되어있는데 0x24부분이   서수배열이기 때문이다.         lea를 쓴이유 끊어져서 있긴때문      
		mov eax, dword ptr[edx]
		add ecx, eax
		mov [ebp-90], ecx											// ordinaltable


		mov eax, dword ptr[ebp + 30]
		mov ecx, [ebp+10]
		lea edx, [ecx + eax + 0x1c]								// 위랑 같은 맥락
		mov eax, dword ptr[edx]
		add ecx, eax
		mov [ebp-80], ecx


		mov eax, [ebp-50]
		mov ecx, [ebp - 90]										// ordinaltable
		movzx edx, word ptr[ecx + eax * 2]							//22  00 11 01   ordinal table에는  이런식으로 저장되어 있으니 2개씩 늘려주는거다.
																						// 참고로 movzx는   word ptr 저장할때 씀    movsz는    byte ptr   이였고

		mov eax, [ebp-80]

		mov ecx, [ebp+10]
		add ecx, dword ptr[eax + edx * 4]							//아까 저장했던 edx*4 즉 4바이트씩 넘긴다.   정수형  배열이라고 생각하면 겁나 편함.
		mov eax, ecx															//winexec의 주소다. eax = 주소

		mov[ebp + 20], 0x63												//그냥 cmd라는 문자열을 ecx에 넣어주는 작업이다.
		mov[ebp + 21], 0x6d
		mov[ebp + 22], 0x64
		mov[ebp + 23], 0
		lea ecx, [ebp + 20]
		push 0																	    // 밑에 2개줄까지 WinExec("cmd",0) 이다.
		push ecx
		call eax
		pop esp															// 몰러 이거 안해주면 뭐시기 뭐시기 런타임 뜬다.
	}
	return 0;
}
