/*
 * Description : It's example module, used for educational ends.
 * Author   : Tony Fabeen
 * Company  : SlimStacks
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

/*
 * Configuration Structs
 */
typedef struct {
  ngx_flag_t  enable;
} ngx_http_devinsampa_out_filter_loc_conf_t;

/**
 * Functions declaration
 */
static ngx_int_t ngx_http_devinsampa_init(ngx_conf_t *conf);
static void * ngx_http_devinsampa_out_filter_create_loc_conf(ngx_conf_t *cf);
static char * ngx_http_devinsampa_out_filter_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_devinsampa_footer_filter(ngx_http_request_t *r, ngx_chain_t *in);

/**
 * Filter function helper
 */
static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;

/*
 * Directives
 */
static ngx_command_t ngx_http_devinsampa_out_filter_commands[] = {
  { ngx_string("devinsampa_output_filter"),
    NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
    ngx_conf_set_flag_slot,
    NGX_HTTP_LOC_CONF_OFFSET,
    offsetof(ngx_http_devinsampa_out_filter_loc_conf_t, enable),
    NULL
  },
  ngx_null_command
};

/**
 * Module Context
 */
static ngx_http_module_t ngx_http_devinsampa_out_filter_module_ctx = {
    NULL,                           /* preconfiguration */
    ngx_http_devinsampa_init,       /* postconfiguration */
    NULL,                           /* create main configuration */
    NULL,                           /* init main configuration */
    NULL,                           /* create server configuration */
    NULL,                           /* merge server configuration */
    ngx_http_devinsampa_out_filter_create_loc_conf, /* create location configuration */
    ngx_http_devinsampa_out_filter_merge_loc_conf   /* merge location configuration */
};

/**
 * Module Definition
 */
ngx_module_t  ngx_http_devinsampa_out_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_devinsampa_out_filter_module_ctx, /* module context */
    ngx_http_devinsampa_out_filter_commands,   /* module directives */
    NGX_HTTP_MODULE,               /* module type */
    NULL,                          /* init master */
    NULL,                          /* init module */
    NULL,                          /* init process */
    NULL,                          /* init thread */
    NULL,                          /* exit thread */
    NULL,                          /* exit process */
    NULL,                          /* exit master */
    NGX_MODULE_V1_PADDING
};


/*
 * Filter initialization
 */
static ngx_int_t ngx_http_devinsampa_init(ngx_conf_t *conf){

  ngx_http_next_body_filter = ngx_http_top_body_filter;
  ngx_http_top_body_filter = ngx_devinsampa_footer_filter;

  return NGX_OK;
}

/*
 * Create loc_conf
 */
static void * ngx_http_devinsampa_out_filter_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_devinsampa_out_filter_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_devinsampa_out_filter_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    return conf;
}

/*
 * Merge loc_conf
 */

static char * ngx_http_devinsampa_out_filter_merge_loc_conf(ngx_conf_t *cf, 
void *parent, void *child){

  ngx_http_devinsampa_out_filter_loc_conf_t *prev = parent;
  ngx_http_devinsampa_out_filter_loc_conf_t *conf = child;

  ngx_conf_merge_value(conf->enable, prev->enable, 0);

  return NGX_CONF_OK;
}


/*
 * Filter function 
 */
static ngx_int_t ngx_devinsampa_footer_filter(ngx_http_request_t *r,
ngx_chain_t *in){

  ngx_buf_t   *buf;
  ngx_chain_t *chain_link, *next_link;
  ngx_uint_t  is_last;

  for (chain_link = in; chain_link; chain_link = chain_link->next){
    if(chain_link->buf->last_buf){
      is_last = 1;
    }
  }

  if(!is_last){
    return ngx_http_next_body_filter(r, in);
  }

  buf = ngx_calloc_buf(r->pool);
  if (buf == NULL){
    return NGX_ERROR;
  }

  buf->pos  = (u_char *) "<h1>Powered by Dev in Sampa Guys !!</h1>";
  buf->last = buf->pos + sizeof("<h1>Powered by Dev in Sampa Guys !!</h1>") - 1;
  buf->start = buf->pos;
  buf->end = buf->last;
  buf->last_buf = 1;
  buf->memory = 1;

  next_link = ngx_alloc_chain_link(r->pool);
  if (next_link == NULL)
    return NGX_ERROR;

  next_link->buf = buf;
  next_link->next = NULL;
  chain_link->next = next_link;
  chain_link->buf->last_buf = 0;

  return ngx_http_next_body_filter(r, in);
}

