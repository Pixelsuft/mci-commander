import os
import sys
import ctypes


kernel32 = ctypes.windll.kernel32
winmm = ctypes.windll.winmm


class MCI:
    def __init__(self, buffer_size: int = 255, encoding: str = sys.getdefaultencoding()) -> None:
        super(MCI, self).__init__()
        self.mci = winmm.mciSendStringA
        self.error = winmm.mciGetErrorStringA
        self.buffer_size = buffer_size
        self.encoding = encoding

    def try_send(self, command: str):
        buffer = ctypes.c_buffer(self.buffer_size)
        error_code = self.mci(command.encode(self.encoding), buffer, self.buffer_size - 1, 0)
        return error_code, (
            self.get_error(error_code) if error_code else buffer.value.decode(self.encoding, errors='replace')
        )

    def get_error(self, error: int):
        buffer = ctypes.c_buffer(self.buffer_size)
        self.error(error, buffer, self.buffer_size - 1)
        return buffer.value.decode(self.encoding, errors='replace')

    def send(self, command: str):
        error, buffer = self.try_send(command)
        if error:
            raise RuntimeError(buffer)
        return buffer


def process_input(mci_obj: MCI, string: str) -> None:
    if not string:
        return
    if string.lower().split(' ')[0] in ('exit', 'quit', 'q'):
        print('Quiting...', end='')
        sys.exit(0)
    if string.lower().split(' ')[0] == 'dir':
        dirs = [x for x in os.listdir() if os.path.isdir(x)]
        files = [x for x in os.listdir() if not os.path.isdir(x)]
        if dirs:
            print('\n'.join(dirs))
        if files:
            print('\n'.join(files))
        return
    if string.lower().split(' ')[0] == 'chdir':
        try:
            os.chdir(' '.join(string.replace('\'', '').replace('"', '').split(' ')[1:]))
        except Exception as err_:
            print(f'Error 0x0: \n{err_}')
        return
    kernel32.SetConsoleTitleW(f'Pixelsuft MCI-Commander ({string})')
    error, buffer = mci_obj.try_send(string)
    if error:
        print(f'Error {hex(error)}: ')
    if buffer.replace('\n', '').strip():
        print(buffer)


def main() -> None:
    try:
        mci = MCI(buffer_size=int(os.getenv('mci_buffer_size') or '1024'))
        [process_input(mci, x) for x in sys.argv[1:]]
        while True:
            kernel32.SetConsoleTitleW('Pixelsuft MCI-Commander')
            user_input = input(f'MCI {os.getcwd()}>')
            process_input(mci, user_input.strip())
    except KeyboardInterrupt:
        print('\nQuiting...', end='')
        sys.exit(0)
    except EOFError:
        print('Quiting...', end='')
        sys.exit(0)


if __name__ == '__main__':
    main()
