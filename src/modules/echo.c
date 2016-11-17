/*
* Copyright (C) Eric Zhang
*/
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/* Module config */
typedef struct {
    ngx_str_t  ed;
} ngx_http_echo_loc_conf_t;

static char *ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void *ngx_http_echo_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_echo_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

/* Directives */
static ngx_command_t  ngx_http_echo_commands[] = {
    
    {   
        ngx_string("echo"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,

        /* 函数指针，当Nginx解析配置文件，碰到指令时，该执行怎样的操作。
         * 而该操作本身，自然是用来设置本模块所对应的ngx_<module name>_conf_t结构体 */
        ngx_http_echo, 
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_echo_loc_conf_t, ed),
        NULL 
    },
    {   
        ngx_string("heno"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_echo,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_echo_loc_conf_t, ed), //因为使用可自带的处理方法，所以必须设置这个
        NULL 
    },

    ngx_null_command
};

/* Http context of the module */
//其中create_loc_conf用于初始化一个配置结构体
//如为配置结构体分配内存等工作；
//merge_loc_conf用于将其父block的配置信息合并到此结构体中，也就是实现配置的继承。这两个函数会被Nginx自动调用
static ngx_http_module_t  ngx_http_echo_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */
    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */
    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
    ngx_http_echo_create_loc_conf,         /* create location configration */
    ngx_http_echo_merge_loc_conf           /* merge location configration */
};

/* Module */
ngx_module_t  ngx_http_echo_module = {
    NGX_MODULE_V1,
    &ngx_http_echo_module_ctx,             /* module context */
    ngx_http_echo_commands,                /* module directives */
    NGX_HTTP_MODULE,                       /* module type NGX_HTTP_MODULE | NGX_CORE_MODULE | NGX_CONF_MODULE | NGX_EVENT_MODULE | NGX_MAIL_MODULE */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

/* Handler function */
static ngx_int_t
ngx_http_echo_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    ngx_buf_t *b;
    ngx_chain_t out;
    ngx_http_echo_loc_conf_t *elcf;

    //获取配置项数据
    elcf = ngx_http_get_module_loc_conf(r, ngx_http_echo_module);

    //对请求方式判断，如果不在处理范围内，返回错误
    if(!(r->method & (NGX_HTTP_HEAD|NGX_HTTP_GET|NGX_HTTP_POST)))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    //设置响应头信息
    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *) "text/html";
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = elcf->ed.len;
    if(r->method == NGX_HTTP_HEAD)
    {
        rc = ngx_http_send_header(r);
        if(rc != NGX_OK)
        {
            return rc;
        }
    }

    //设置响应body
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if(b == NULL)
    {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Failed to allocate response buffer.");
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    out.buf = b;
    out.next = NULL; //标记当前为最后数据
    b->pos = elcf->ed.data;
    b->last = elcf->ed.data + (elcf->ed.len);
    b->memory = 1;
    b->last_buf = 1; //标记
    rc = ngx_http_send_header(r);
    if(rc != NGX_OK)
    {
        return rc;
    }
    return ngx_http_output_filter(r, &out);
}


//参数转化函数，设置handler
static char *
ngx_http_echo(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_echo_handler;
    ngx_conf_set_str_slot(cf, cmd, conf);
    return NGX_CONF_OK;
}

static void *
ngx_http_echo_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_echo_loc_conf_t  *conf;
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_echo_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    conf->ed.len = 0;
    conf->ed.data = NULL;
    return conf;
}
static char *
ngx_http_echo_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_echo_loc_conf_t *prev = parent;
    ngx_http_echo_loc_conf_t *conf = child;
    ngx_conf_merge_str_value(conf->ed, prev->ed, "");
    return NGX_CONF_OK;
}
