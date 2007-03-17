sub create_toolbar ()
{
	my $toolbar = Gtk2::Toolbar->new ();

	$toolbar->insert_stock ("gtk-go-back",
							'Back',
							'tt private text',
							\&back_cb,
							'user_data',
							0);

	$toolbar->insert_stock ("gtk-go-forward",
							'Forward',
							'tt private text',
							\&fwd_cb,
							'user_data',
							1);

	$toolbar->insert_stock ("gtk-go-up",
							'Up',
							'tt private text',
							\&up_cb,
							'user_data',
							2);

	$toolbar->insert_space (3);

	$toolbar->insert_stock ("gtk-cut",
							'Cut',
							'tt private text',
							\&cut_cb,
							'user_data',
							4);

	$toolbar->insert_stock ("gtk-copy",
							'Copy',
							'tt private text',
							\&copy_cb,
							'user_data',
							5);

	$toolbar->insert_stock ("gtk-paste",
							'Paste',
							'tt private text',
							\&paste_cb,
							'user_data',
							6);

	$toolbar->insert_stock ("gtk-delete",
							'Delete',
							'tt private text',
							\&delete_cb,
							'user_data',
							7);

	return $toolbar;
}

1;
