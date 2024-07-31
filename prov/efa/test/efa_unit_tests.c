/* SPDX-License-Identifier: BSD-2-Clause OR GPL-2.0-only */
/* SPDX-FileCopyrightText: Copyright Amazon.com, Inc. or its affiliates. All rights reserved. */

#include "efa_unit_tests.h"

struct efa_env orig_efa_env = {0};

/* Runs once before all tests */
static int efa_unit_test_mocks_group_setup(void **state)
{
	struct efa_resource *resource;
	resource = calloc(1, sizeof(struct efa_resource));
	*state = resource;

	orig_efa_env = efa_env;

	return 0;
}

/* Runs once after all tests */
static int efa_unit_test_mocks_group_teardown(void **state)
{
	struct efa_resource *resource = *state;
	free(resource);

	return 0;
}

/* Runs before every test */
static int efa_unit_test_mocks_setup(void **state)
{
	/* Zero out *resource */
	struct efa_resource *resource = *state;
	memset(resource, 0, sizeof(struct efa_resource));

	return 0;
}

/* Runs after every test */
static int efa_unit_test_mocks_teardown(void **state)
{
	struct efa_resource *resource = *state;

	efa_unit_test_resource_destruct(resource);

	efa_ibv_submitted_wr_id_vec_clear();

	g_efa_unit_test_mocks = (struct efa_unit_test_mocks) {
		.dummy_address = -1,
		.local_host_id = 0,
		.peer_host_id = 0,
		.ibv_create_ah = __real_ibv_create_ah,
		.efadv_query_device = __real_efadv_query_device,
		.ibv_reg_mr_calls = 0,
		.ibv_reg_mr_fn = __real_ibv_reg_mr,
		.ibv_reg_mr_iova2 = __real_ibv_reg_mr_iova2,
		.ibv_dereg_mr = __real_ibv_dereg_mr,
#if HAVE_EFADV_CQ_EX
		.efadv_create_cq = __real_efadv_create_cq,
#endif
		.ofi_copy_from_hmem_iov = __real_ofi_copy_from_hmem_iov,
		.ibv_is_fork_initialized = __real_ibv_is_fork_initialized,
#if HAVE_EFA_DMABUF_MR
		.ibv_reg_dmabuf_mr = __real_ibv_reg_dmabuf_mr,
#endif
	};

	/* Reset environment */
	efa_env = orig_efa_env;
	unsetenv("FI_EFA_FORK_SAFE");
	unsetenv("FI_EFA_USE_DEVICE_RDMA");

	return 0;
}

int main(void)
{
	int ret;
	/* Requires an EFA device to work */
	const struct CMUnitTest efa_unit_tests[] = {
		cmocka_unit_test_setup_teardown(test_av_insert_duplicate_raw_addr, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_av_insert_duplicate_gid, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_device_construct_error_handling, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_ignore_missing_host_id_file, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_has_valid_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_ignore_short_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_ignore_non_hex_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_handshake_receive_and_send_valid_host_ids_with_connid, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_handshake_receive_and_send_valid_host_ids_without_connid, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_handshake_receive_valid_peer_host_id_and_do_not_send_local_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_handshake_receive_without_peer_host_id_and_do_not_send_local_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_getopt_undersized_optlen, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_getopt_oversized_optlen, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_setopt_shared_memory_permitted, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_enable_qp_in_order_aligned_128_bytes_good, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_enable_qp_in_order_aligned_128_bytes_bad, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_pkt_pool_flags, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_pkt_pool_page_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_dc_atomic_queue_before_handshake, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_dc_send_queue_before_handshake, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_dc_send_queue_limit_before_handshake, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_read_queue_before_handshake, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_write_queue_before_handshake, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_read_copy_pkt_pool_128_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_send_with_shm_no_copy, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_rma_without_caps, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_atomic_without_caps, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_user_zcpy_rx_happy, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_user_zcpy_rx_unhappy_due_to_sas, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_close_discard_posted_recv, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ep_zcpy_recv_cancel, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_dgram_cq_read_empty_cq, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_ibv_cq_ex_read_empty_cq, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_ibv_cq_ex_read_failed_poll, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_cq_create_error_handling, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_cq_read_bad_send_status_unresponsive_receiver, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_cq_read_bad_send_status_unresponsive_receiver_missing_peer_host_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_cq_read_bad_send_status_invalid_qpn, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_cq_read_bad_send_status_message_too_long, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_ibv_cq_ex_read_bad_recv_status, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_ibv_cq_ex_read_recover_forgotten_peer_ah, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_ibv_cq_ex_read_ignore_removed_peer, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_rdm_fallback_to_ibv_create_cq_ex_cq_read_ignore_forgotton_peer, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_open_ep_with_wrong_info, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_open_ep_with_api_1_1_info, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_msg_order_rdm_order_none, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_msg_order_rdm_order_sas, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_msg_order_dgram_order_none, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_msg_order_dgram_order_sas, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_op_flags_rdm, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_tx_rx_size_rdm, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_check_shm_info_hmem, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_check_shm_info_op_flags, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_check_shm_info_threading, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_info_check_hmem_cuda_support_on_api_lt_1_18, NULL, NULL),
		cmocka_unit_test_setup_teardown(test_info_check_hmem_cuda_support_on_api_ge_1_18, NULL, NULL),
		cmocka_unit_test_setup_teardown(test_info_check_no_hmem_support_when_not_requested, NULL, NULL),
		cmocka_unit_test_setup_teardown(test_efa_nic_select_all_devices_matches, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_nic_select_first_device_matches, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_nic_select_first_device_with_surrounding_comma_matches, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_nic_select_first_device_first_letter_no_match, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_nic_select_empty_device_no_match, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env1_opt1, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env0_opt0, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env1_opt0, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env0_opt1, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env1, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_env0, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_opt1, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_opt0, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_use_device_rdma_opt_old, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_srx_min_multi_recv_size, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_srx_cq, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_srx_lock, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rnr_queue_and_resend, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_prepare_to_post_send_with_no_enough_tx_pkts, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_prepare_to_post_send_host_memory, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_prepare_to_post_send_host_memory_align128, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_prepare_to_post_send_cuda_memory, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_prepare_to_post_send_cuda_memory_align128, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_ope_post_write_0_byte, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_msg_send_to_local_peer_with_null_desc, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_fork_support_request_initialize_when_ibv_fork_support_is_needed, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_fork_support_request_initialize_when_ibv_fork_support_is_unneeded, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_no_enough_runt, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_smaller_than_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_exceeding_total_len, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_normal, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_host_memory_exceeding_total_len, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_host_memory_normal, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_128_multiple_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_non_128_multiple_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_smaller_than_128_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_get_runt_size_cuda_memory_exceeding_total_len_128_alignment, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_select_readbase_rtm_no_runt, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_select_readbase_rtm_do_runt, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_pke_get_available_copy_methods_align128, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_domain_open_ops_wrong_name, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_domain_open_ops_mr_query, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_cq_ibv_cq_poll_list_same_tx_rx_cq_single_ep, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_cq_ibv_cq_poll_list_separate_tx_rx_cq_single_ep, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_cq_post_initial_rx_pkts, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_cntr_ibv_cq_poll_list_same_tx_rx_cq_single_ep, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_cntr_ibv_cq_poll_list_separate_tx_rx_cq_single_ep, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_cntr_post_initial_rx_pkts, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_system_always_ibv_reg_mr, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_cuda_dmabuf_support_always_ibv_reg_mr, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_cuda_dmabuf_support_ibv_reg_mr_fail_fallback_keygen, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_cuda_dmabuf_support_require_dmabuf_fail_no_fallback, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_neuron_dmabuf_support_dmabuf_success, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_neuron_dmabuf_support_get_fd_fail_fallback, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_neuron_dmabuf_support_mr_fail_no_fallback, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_neuron_dmabuf_support_require_dmabuf_fail_no_fallback, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_synapseai_dmabuf_support_fd_fail_no_fallback, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_reorder_expected_msg_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_reorder_smaller_msg_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_reorder_larger_msg_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_reorder_overflow_msg_id, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_move_overflow_pke_to_recvwin, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_keep_pke_in_overflow_list, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
		cmocka_unit_test_setup_teardown(test_efa_rdm_peer_append_overflow_pke_to_recvwin, efa_unit_test_mocks_setup, efa_unit_test_mocks_teardown),
	};

	cmocka_set_message_output(CM_OUTPUT_XML);

	ret = cmocka_run_group_tests_name("efa unit tests", efa_unit_tests, efa_unit_test_mocks_group_setup, efa_unit_test_mocks_group_teardown);

	return ret;
}
