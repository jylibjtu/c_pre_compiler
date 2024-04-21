@Override
	public Object[] getChildren(Object element) {
		if(element != null){
			if(element instanceof CarInfoImpl){
				CarInfoImpl parent = (CarInfoImpl)element;
				ResultListKey key = new ResultListKey(parent.getRecid());
				List<ICarInfo> list = context.getList(ICarInfo.class , key);
				if(list != null && list.size() > 0){
					return list.toArray();
				}
			}
		} else {
			List<ICarInfo> list = context.getList(ICarInfo.class);
			if(list != null && list.size() > 0){
				return list.toArray();
			}
		}
		return null;
	}
