ngx_http_wait_request_handler
	ngx_http_process_request_line
		ngx_http_read_request_header
		ngx_http_parse_request_line

		ngx_http_process_request_headers
			ngx_http_parse_header_line
			ngx_http_process_request
				ngx_http_handler
					ngx_http_core_run_phases（调用所有的checker）
				ngx_http_run_posted_requests


ngx_http_block -》 ngx_http_init_phase_handlers 设置相关的phase checker
ngx_http_block 在http命令里
static ngx_command_t  ngx_http_commands[] = {

    { ngx_string("http"),
      NGX_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_NOARGS,
      ngx_http_block,
      0,
      0,
      NULL },

      ngx_null_command
};

所以在遇见命令http的时候就调用这个set（ngx_http_block）设置好了相关的phase-checker